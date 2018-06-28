java 集合 2 - LinkedList

参考文章：[图解集合2：LinkedList](http://www.cnblogs.com/xrq730/p/5005347.html)

LinkedList是基于[双向链表](https://baike.baidu.com/item/%E5%8F%8C%E5%90%91%E9%93%BE%E8%A1%A8/2968731?fr=aladdin)实现的。
|特性||
|----|-----|
|是否存取null值|可以|
|元素是否可以重复|可以|
|是否有序|有序|
|是否线程安全|不安全|

接下来直接通过源码（**jdk1.8**）分析。

### 结点定义
```java

    private static class Node<E> {
        E item;
        Node<E> next;
        Node<E> prev;

        Node(Node<E> prev, E element, Node<E> next) {
            this.item = element;
            this.next = next;
            this.prev = prev;
        }
    }
```
可见，对于每一个结点，可以通过prev找到它的直接前驱，通过next找到其后继。
### 头，尾结点
```java
    /**
     * Pointer to first node.
     * Invariant: (first == null && last == null) ||
     *            (first.prev == null && first.item != null)
     */
    transient Node<E> first;

    /**
     * Pointer to last node.
     * Invariant: (first == null && last == null) ||
     *            (last.next == null && last.item != null)
     */
    transient Node<E> last;
```
作为双向链表，链表头的前驱为链表尾，链表尾的后继为链表头，first和last满足如下关系：
first不为空，且链表尾为空，那么链表只有一个元素，保存于链表头；last不为空，且链表头为空，那么链表只有一个元素，保存于链表尾。

### add方法
```java
    public boolean add(E e) {
        linkLast(e);
        return true;
    }
    
    public void add(int index, E element) {
        checkPositionIndex(index);

        if (index == size)
            linkLast(element);
        else
            linkBefore(element, node(index));
    }
``` 
- 一个参数的add方法实际调用了linkLast方法，linkLast方法。
- 两个参数的add方法先检查下标是否越界，之后如果下标（从0开始）为当前链表的长度（size从1开始），那么就是新增元素于链表尾，否则调用linkbefore插入元素。

#### linkLast方法
```java
    /**
     * Links e as last element.
     */
    void linkLast(E e) {
        final Node<E> l = last;
        final Node<E> newNode = new Node<>(l, e, null);
        last = newNode;
        if (l == null)
            first = newNode;
        else
            l.next = newNode;
        size++;
        modCount++;
    }
```
可见linkLast方法会将新的结点进行构造，然后替换原来的尾结点，如果原来的尾结点为空，那意味着链表还没有元素（有一个元素时头尾就都不为null），此时构造的就是第一个元素，使头尾都指向同一元素（该元素的前驱和后继都为null），如果原来的尾结点已经存在，那么将原来的尾结点的后继指向新的尾结点即可。

#### linkBefore方法
```java
    /**
     * Inserts element e before non-null Node succ.
     */
    void linkBefore(E e, Node<E> succ) {
        // assert succ != null;
        final Node<E> pred = succ.prev;
        final Node<E> newNode = new Node<>(pred, e, succ);
        succ.prev = newNode;
        if (pred == null)
            first = newNode;
        else
            pred.next = newNode;
        size++;
        modCount++;
    }
```
这里对应succ参数的定位发生在node方法中：
#### node方法
```java
    /**
     * Returns the (non-null) Node at the specified element index.
     */
    Node<E> node(int index) {
        // assert isElementIndex(index);

        if (index < (size >> 1)) {
            Node<E> x = first;
            for (int i = 0; i < index; i++)
                x = x.next;
            return x;
        } else {
            Node<E> x = last;
            for (int i = size - 1; i > index; i--)
                x = x.prev;
            return x;
        }
    }
```
结合上面双参数的add方法，可见该方法首先会判断新插入的点在链表中的大体位置（前一半还是后一半），然后从前往后或从后往前定位到指定下标的点并返回。

再回头看linkBefore方法，构建新节点，修改被插入结点的前驱，被插入结点的原前驱的后继。

### get方法
```java
public E get(int index) {
        checkElementIndex(index);
        return node(index).item;
    }
```
get方法首先会检查下标，然后通过node方法定位目标结点，取出值返回。

### indexOf方法
```java
 public int indexOf(Object o) {
        int index = 0;
        if (o == null) {
            for (Node<E> x = first; x != null; x = x.next) {
                if (x.item == null)
                    return index;
                index++;
            }
        } else {
            for (Node<E> x = first; x != null; x = x.next) {
                if (o.equals(x.item))
                    return index;
                index++;
            }
        }
        return -1;
    }
```
该方法用于查找指定元素的下标，可见该方法会对目标元素是否为null采用不同的搜索方式，没找到则返回-1。

