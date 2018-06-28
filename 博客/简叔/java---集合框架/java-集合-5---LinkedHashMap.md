参考文章：[Java LinkedHashMap 源码解析](https://mp.weixin.qq.com/s?__biz=MjM5NzMyMjAwMA==&mid=2651479784&idx=1&sn=929e81473146994b15404f4f4d15748e&chksm=bd2532978a52bb81947307b772b39d89566d84a35f6fe55956bb516a357dc92a27e3a498c64c&mpshare=1&scene=23&srcid=0116WyUGZhbE9kiPwYGd0x42#rd)，[图解集合6：LinkedHashMap](http://www.cnblogs.com/xrq730/p/5052323.html)

基于 jdk 1.8 分析源码

LinkedHashMap允许一个null键和多个null值，有序，线程不安全。

LinkedHashMap直接使用父类HashMap的**数组&链表**结构保存数据，同时自己又采用了**环型双向链表**来维护元素顺序。

LinkedHashMap提供了两种形式的顺序：
**访问顺序（access order）**：非常实用，可以使用这种顺序实现LRU（Least Recently Used）缓存（详见get方法解析）
**插入顺序（insertion orde）**：元素的插入顺序（同一key的多次插入不会影响其顺序，详见newNode方法解析）

先来看看这两种顺序是的效果：
### 访问顺序
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-54a966c473ee7544?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
可见通过get方法访问了Map中的元素，之后遍历值集合时遍历顺序受到了访问操作的影响，即“最近”被访问的元素排在值集合（即链表）的最前面。

这样在实现LRU算法时就能很方便的找到最近最少被使用的元素，即链表的头元素。

### 插入顺序
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-9c4d5c33ae276201?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
默认采用的就是“插入顺序”。遍历时就会以插入顺序遍历。

## Entry
```java
    /**
     * HashMap.Node subclass for normal LinkedHashMap entries.
     */
    static class Entry<K,V> extends HashMap.Node<K,V> {
        Entry<K,V> before, after;
        Entry(int hash, K key, V value, Node<K,V> next) {
            super(hash, key, value, next);
        }
    }
```
LinkedHashMap直接使用了HashMap的数据结构来保存元素，不同的是相比HashMap的Entry增加了before和after成员，这两个成员用于维护元素的顺序。

## 构造器
```java
    public LinkedHashMap() {
        super();
        accessOrder = false;
    }
    
    public LinkedHashMap(int initialCapacity,
                         float loadFactor,
                         boolean accessOrder) {
        super(initialCapacity, loadFactor);
        this.accessOrder = accessOrder;
    }
```
LinkedHashMap继承自HashMap，所以构造其中的super调用了父类HashMap的构造器。

要想使用“访问顺序”形式时只能通过调用三个参数的构造器创建实例。

## put
LinkedHashMap并未重写HashMap的put方法，即直接调用了HashMap的put方法，也就是说LinkedHashMap插入元素时并未采取额外措施以实现其“有序”功能，那么该功能是在哪里实现的呢？

## get
```java
    public V get(Object key) {
        Node<K,V> e;
        if ((e = getNode(hash(key), key)) == null)
            return null;
        if (accessOrder)
            afterNodeAccess(e);
        return e.value;
    }
```
get方法用于获得指定键对应的值（如果存在的话）。

注意`if(accessOrder) afterNodeAccess(e);`一句，上面提到LinkedHashMap的“有序”有两种体现形式，一种为“插入顺序”，一种为“访问顺序”，而`accessOrder`成员就是“访问顺序”形式的启用开关。

## afterNodeAccess
```java
    void afterNodeAccess(Node<K,V> e) { // move node to last
        LinkedHashMap.Entry<K,V> last;
        if (accessOrder && (last = tail) != e) {
            LinkedHashMap.Entry<K,V> p =
                (LinkedHashMap.Entry<K,V>)e, b = p.before, a = p.after;
            p.after = null;
            if (b == null) 
                head = a;
            else 
                b.after = a; 
            if (a != null) 
                a.before = b;
            else 
                last = b; 
            if (last == null)
                head = p;
            else {
                p.before = last;
                last.after = p;
            }
            tail = p;
            ++modCount;
        }
    }
```
如果开启了“访问顺序”的形式（创建实例时使用三个参数的构造器），并且当前访问的元素不是链表的尾结点，那么就会将当前访问结点变换为尾结点。

即最外层if条件成立，之后将当前访问结点强转为记录有前驱后继的LinkedHashMap专用结点，这里需要注意：参看上面的put方法我们知道put方法直接调用了HashMap的put方法，那么按理插入的结点类型也是HashMap.Node类型的，怎么能够强转为LinkedHashMap.Entry类型呢？

**原因是这样的**：参考HashMap源码可以知道，put方法内部调用了putVal，putVal中创建新节点调用的是`newNode`方法（而不是直接 new HashMap.Node），而LinkedHashMap重写了这个方法：

HashMap#newNode
```java
    Node<K,V> newNode(int hash, K key, V value, Node<K,V> next) {
        return new Node<>(hash, key, value, next);
    }
```

LinkedHashMap#newNode
```java
    Node<K,V> newNode(int hash, K key, V value, Node<K,V> e) {
        LinkedHashMap.Entry<K,V> p =
            new LinkedHashMap.Entry<K,V>(hash, key, value, e);
        linkNodeLast(p);
        return p;
    }
```
所以这里强转就不会出现`ClassCastException`。

之后的步骤就是将当前访问结点变换为尾结点的过程了（有点复杂，没理清）。

## newNode
```java
    Node<K,V> newNode(int hash, K key, V value, Node<K,V> e) {
        LinkedHashMap.Entry<K,V> p =
            new LinkedHashMap.Entry<K,V>(hash, key, value, e);
        linkNodeLast(p);
        return p;
    }
```
上面已经说了一下newNode方法的，除了用于创建新元素以外，在LinkedHashMap中，newNode方法还额外做了一件事情：将当前元素链接到链表尾，即创建LinkedHashMap专用的LinkedHashMap.Entry，构造该节点的前驱和后继以维护其“插入顺序”。对应linkNodeLast方法。

```java
 private void linkNodeLast(LinkedHashMap.Entry<K,V> p) {
        LinkedHashMap.Entry<K,V> last = tail;
        tail = p;
        if (last == null)
            head = p;
        else {
            p.before = last;
            last.after = p;
        }
    }
```
如果尾结点为空，这意味着LinkedHashMap中没有元素，那么新元素就是链表头（`head=p`），同时链表尾元素也指向它（`tail=p`）；否则将链表尾元素替换为新的元素（`p.before=last;last.after=p;`）。
