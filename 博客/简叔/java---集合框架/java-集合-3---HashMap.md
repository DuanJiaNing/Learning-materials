
参考文章：[什么是HashMap？](https://mp.weixin.qq.com/s?__biz=MzAxOTc0NzExNg==&mid=2665514069&idx=1&sn=2996d864bbe596d0af763fba3d244fa7&chksm=80d67c16b7a1f500ec6a191eb4a0beac0e95dbd5a7bf8ee01f5ed2cb17960b9ab32c0b965949&mpshare=1&scene=23&srcid=1201V2GnmuBvSQ8zSCHJ20jV#rd)、[图解集合4：HashMap](http://www.cnblogs.com/xrq730/p/5030920.html)

接下来通过 jdk 1.8 进行分析

HashMap是一个用于存储Key-Value键值对的集合，每一个键值对也叫做Entry。这些个键值对（Entry）分散存储在一个数组当中，这个数组就是HashMap的主干。

**特点：**
1. HashMap的key和value都允许null值，当key相同时， 值会覆盖，即key是唯一的。
2. HashMap是无序的，存值时是根据key的hashcode进行映射，因而存入顺序在数组中的实际顺序会被打乱。
3. 线程不安全

## 构造器
```java
    static final float DEFAULT_LOAD_FACTOR = 0.75f;

    // 只选取了两个较常用的构造器
    public HashMap(int initialCapacity) {
        this(initialCapacity, DEFAULT_LOAD_FACTOR);
    }
    public HashMap() {
        this.loadFactor = DEFAULT_LOAD_FACTOR; // all other fields defaulted
    }
```
loadFactor指加载因子，加载因子是哈希表在其容量自动增加之前可以达到多满的一种尺度。当哈希表中的条目数超出了加载因子与当前容量的乘积时，通过调用 resize方法将容量翻倍。

可见默认的加载因子为0.75，即HashMap中元素个数超出总大小的75%时，容量将扩大。

## 数组实体
```java

    /**
     * The table, initialized on first use, and resized as
     * necessary. When allocated, length is always a power of two.
     * (We also tolerate length zero in some operations to allow
     * bootstrapping mechanics that are currently not needed.)
     */
    transient Node<K,V>[] table;
```
注释：数组实体，第一次使用时初始化，必要时进行扩容，当分配容量时，数组大小始终为2的幂。

## 数组元素数据结构
```java
    static class Node<K,V> implements Map.Entry<K,V> {
        final int hash;
        final K key;
        V value;
        Node<K,V> next;

        Node(int hash, K key, V value, Node<K,V> next) {
            this.hash = hash;
            this.key = key;
            this.value = value;
            this.next = next;
        }
    ....
    }
```
可见每一个数组元素都为一个链表结构。

## put方法
```java
    public V put(K key, V value) {
        return putVal(hash(key), key, value, false, true);
    }
```
put方法内部调用了putVal方法，在putVal方法中执行具体的插入逻辑。
其中hash方法对key做了如下处理：
`return (key == null) ? 0 : (h = key.hashCode()) ^ (h >>> 16)`
该操作称之为“扰动函数”，旨在使键的hash值固定在一定范围，减少hash冲突。

假设有key的hashCode为3029737，其对应的32为二进制数据为：
0000 0000 0010 1110 0011 1010 1110 1001，通过“扰动函数”，其最终hash为：
0000 0000 0000 0000 0000 0000 0000 **1010**
参考文章：[JDK 源码中 HashMap 的 hash 方法原理是什么](https://www.zhihu.com/question/20733617/answer/111577937)

![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-9a081a819287267a?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## putVal方法
```java
    final V putVal(int hash, K key, V value, boolean onlyIfAbsent,
                   boolean evict) {
        Node<K,V>[] tab; Node<K,V> p; int n, i;
        if ((tab = table) == null || (n = tab.length) == 0)
            n = (tab = resize()).length;
        if ((p = tab[i = (n - 1) & hash]) == null)
            tab[i] = newNode(hash, key, value, null);
        else {
            Node<K,V> e; K k;
            if (p.hash == hash &&
                ((k = p.key) == key || (key != null && key.equals(k))))
                e = p;
            else if (p instanceof TreeNode)
                e = ((TreeNode<K,V>)p).putTreeVal(this, tab, hash, key, value);
            else {
                for (int binCount = 0; ; ++binCount) {
                    if ((e = p.next) == null) {
                        p.next = newNode(hash, key, value, null);
                        if (binCount >= TREEIFY_THRESHOLD - 1) // -1 for 1st
                            treeifyBin(tab, hash);
                        break;
                    }
                    if (e.hash == hash &&
                        ((k = e.key) == key || (key != null && key.equals(k))))
                        break;
                    p = e;
                }
            }
            if (e != null) { // existing mapping for key
                V oldValue = e.value;
                if (!onlyIfAbsent || oldValue == null)
                    e.value = value;
                afterNodeAccess(e);
                return oldValue;
            }
        }
        ++modCount;
        if (++size > threshold)
            resize();
        afterNodeInsertion(evict);
        return null;
    }
```
先看最上面的if判断，该判断用于判断数组实体是否为null或者数组长度为0，两种情况下都需要对数组进行扩容，扩容后给局部变量tab和n从新赋值。
`if ((tab = table) == null || (n = tab.length) == 0)
            n = (tab = resize()).length;
`

注意`tab[i=(n-1)&hash])`，这里n为数组长度，`(n-1)&hash`计算出指定key对应元素在数组实体中的下标。之后取得该位置元素并赋值给局部变量p。

如果p为null，即当前下标对应元素（链表）还没有元素，直接将新元素放当前位置即可，此时没有hash冲突。

如果p不为null，意味着发生了hash冲突，需要进一步检查。

`if (p.hash == hash && ((k = p.key) == key || (key != null && key.equals(k))))`判断：如果链表头元素的hash与新值hash相等并且头元素的键与新键相同（== 用于比较内存地址）；不同的key由hash方法得到的hash值可能相等，但两个key并不相同，即除了hash值要相同外，还需用==比较两个key的内存地址，确认两个key其实指向堆上的同一个实例。这个时候才确认为重复key，才能用新值替换旧值。另一种情况是新键不为null并且两个key的equals返回true，也确认为重复key。

`else if (p instanceof TreeNode)`如果结点的链表结构为红黑树（下一个`else`分支控制了在何种情况下会转化为红黑树），则通过putTreeVal方法查找是否为重复key。

`else`，排除了上面的两种情况（新键与链表头结点重复，链表为红黑树），即表示新键可能在头结点之后的某一个结点重复；遍历链表结点，如果下一个结点为null，那表示已经遍历到链表尾，也就表示没有找到重复的键，直接创建新结点，插入链表尾即可，接下来还需判断如果遍历结点个数大于`TREEIFY_THRESHOLD`（其值为8），则将链表转为红黑树（通过treeifybBin方法），这里隐含当前else分支只有在链表个数小于8时才会执行（如果结点为红黑树就在上一个判断分支截断了），之后结束遍历；另一种情况如果当前链表结点不是尾结点，则判断key是否重复，重复就结束遍历。

在上面的*“找重复key”*结束后，判断e值，不为null表示找到重复key，如果`onlyIfAbsent`参数为false或重复key对应的值为null，就替换重复key的值，并返回被替换的值，整个方法结束。

元素插入的逻辑到这里就结束了。

最后判断数组容量，如果实际元素个数大于容量 * 加载因子，就扩容。

其中`afterNodeAccess`，`afterNodeInsertion`为回调方法，子类可实现这些方法以接收动作回调。

## get方法
```java
public V get(Object key) {
        Node<K,V> e;
        return (e = getNode(hash(key), key)) == null ? null : e.value;
    }
```
get方法调用的是getNode方法，找到Node的或就获取其value返回，否则为null，即找不到值。

## getNode方法
```java
    final Node<K,V> getNode(int hash, Object key) {
        Node<K,V>[] tab; Node<K,V> first, e; int n; K k;
        if ((tab = table) != null && (n = tab.length) > 0 &&
            (first = tab[(n - 1) & hash]) != null) {
            if (first.hash == hash && // always check first node
                ((k = first.key) == key || (key != null && key.equals(k))))
                return first;
            if ((e = first.next) != null) {
                if (first instanceof TreeNode)
                    return ((TreeNode<K,V>)first).getTreeNode(hash, key);
                do {
                    if (e.hash == hash &&
                        ((k = e.key) == key || (key != null && key.equals(k))))
                        return e;
                } while ((e = e.next) != null);
            }
        }
        return null;
    }
```
查看get方法知道参数hash通过hash(Object key)方法计算。

`if ((tab = table) != null && (n = tab.length) > 0 &&
            (first = tab[(n - 1) & hash]) != null)`
这个判断语句排除了数组实体为null（未put就get），数组长度为0以及hash值对应下标元素为null的情况。

之后若进入第一个if分支，表示上述条件满足。之后优先检查链表头元素 ，是目标元素直接返回，查找结束，否则遍历查找（分普通链表和红黑树链表两种查找情况）。

## 线程安全性（高并发死循环导致死锁）
参考文章：[Java 8系列之重新认识HashMap](http://blog.csdn.net/qq_27093465/article/details/52207135)

HashMap是线程不安全的，如果需要线程安全，应使用ConcurrentHashMap。

在 jdk 1.7 的环境下，map初始化为一个长度为2的数组，loadFactor=0.75，threshold=2*0.75=1，也就是说当put第二个key的时候，map就需要进行resize，在并发的情况下极有可能**将链表错误构造为双端链表**，此时通过get方法查找一个不存在的key，get方法中通过`do{..}while((e = e.next) != null)`遍历查找key，双端队列导致`e != null`始终为true，死循环就出现了。
