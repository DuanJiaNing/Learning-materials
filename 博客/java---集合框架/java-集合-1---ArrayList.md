java 集合 1 - ArrayList

参考文章：[图解集合1：ArrayList](http://www.cnblogs.com/xrq730/p/4989451.html)

ArrayList使用的数据结构是数组，即使用数组保存元素。
|特性||
|----|-----|
|是否存取null值|可以|
|元素是否可以重复|可以|
|是否有序|有序|
|是否线程安全|不安全|

接下来直接通过源码（**jdk1.8**）分析。

## 可克隆、随机访问、序列化
先来看看ArrayList的定义：
```java
public class ArrayList<E> extends AbstractList<E>
        implements List<E>, RandomAccess, Cloneable, java.io.Serializable
```
- 可见实现了RandomAccess接口，这是一个标志接口，表示可以对该集合执行随机存取操作。
- 实现了Clonable接口，并且覆写了Object的clone方法（若实现了接口却不覆盖该方法而直接调用clone方法将受到`CloneNotSupportedException`异常），表明该集合可以被复制，克隆。
- 实现了Serializable接口，且覆写了readObject和writeObject方法，表明可被序列化。

### 数组实体
```java

    /**
     * The array buffer into which the elements of the ArrayList are stored.
     * The capacity of the ArrayList is the length of this array buffer. Any
     * empty ArrayList with elementData == DEFAULTCAPACITY_EMPTY_ELEMENTDATA
     * will be expanded to DEFAULT_CAPACITY when the first element is added.
     */
    transient Object[] elementData; // non-private to simplify nested class access
```
elementData为ArrayList中存放元素的实体，该成员变量被transient修饰，表示在进行序列化时忽略该变量（不序列化该成员）。这样修饰的原因可以在writeObject和readObject方法中找到。

### clone 方法
```java
    /**
     * Returns a shallow copy of this <tt>ArrayList</tt> instance.  (The
     * elements themselves are not copied.)
     *
     * @return a clone of this <tt>ArrayList</tt> instance
     */
    public Object clone() {
        try {
            ArrayList<?> v = (ArrayList<?>) super.clone(); // 1
            v.elementData = Arrays.copyOf(elementData, size); // 2
            v.modCount = 0;
            return v;
        } catch (CloneNotSupportedException e) {
            // this shouldn't happen, since we are Cloneable
            throw new InternalError(e);
        }
    }
```
注释：返回当前对象的浅拷贝，集合的元素自身将不会进行拷贝。

第1句将在堆中新建一个ArrayList对象，第2句将当前对象的数组实体复制给新的集合，这里**`Arrays.copyOf`方法只将数组实体中元素的引用挨个复制给新集合的数组**（浅拷贝的原因在此，只复制了元素的引用，元素自身并没有拷贝）。

### writeObject 方法
```java
private void writeObject(java.io.ObjectOutputStream s)
        throws java.io.IOException{
        // Write out element count, and any hidden stuff
        int expectedModCount = modCount;
        s.defaultWriteObject();

        // Write out size as capacity for behavioural compatibility with clone()
        s.writeInt(size);

        // Write out all elements in the proper order.
        for (int i=0; i<size; i++) {
            s.writeObject(elementData[i]);
        }

        if (modCount != expectedModCount) {
            throw new ConcurrentModificationException();
        }
    }
```
在将ArrayList序列化到文件中时通过该方法决定对象中的哪些成员需要被序列化，主要看`// Write out all elements in the proper order.`注释的下一个for循环，注释意思为将所有元素写入（e.g. 文件）到正确的位置，这与readObject（从文件中反序列化）是对应的。

ArrayList中存储元素的数组并不是时时刻刻都被完全使用的，比如数组大小为10，但实际只有4个元素，剩余的6个元素为null，而writeObject方法中for循环只将实际存储的4个元素（size个）进行序列化，即那些为null的元素是不进行序列化的，相反，如果不覆写writeObject方法，elementData也不使用transient修饰，那么在进行序列化时将使用默认的序列化方法处理，6个null元素也将被序列化，占用一定设备资源，从而造成浪费。

### readObject 方法
```java
/**
     * Reconstitute the <tt>ArrayList</tt> instance from a stream (that is,
     * deserialize it).
     */
    private void readObject(java.io.ObjectInputStream s)
        throws java.io.IOException, ClassNotFoundException {
        elementData = EMPTY_ELEMENTDATA;

        // Read in size, and any hidden stuff
        s.defaultReadObject();

        // Read in capacity
        s.readInt(); // ignored

        if (size > 0) {
            // be like clone(), allocate array based upon size not capacity
            ensureCapacityInternal(size);

            Object[] a = elementData;
            // Read in all elements in the proper order.
            for (int i=0; i<size; i++) {
                a[i] = s.readObject();
            }
        }
    }
```
重点在` // Read in all elements in the proper order.`注释的下一个for循环，与writeObject对应，这里只需反序列化size个元素。

### add 方法
![这里写图片描述](http://img.blog.csdn.net/20180104142534775?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvYWltZWltZWlUUw==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
一个参数的add方法先对数组实体容量进行确定，然后直接将新元素添加到数组最后。

两个参数的add方法用于在指定下标插入新元素，在插入之前先检查目标下标是否越界（越界将收到IndexOutOfBoundsException）异常，之后确定容量，最后将元素插入指定下标。

### 数组初始大小及扩容策略

#### 三种初始化方式
1. `ArrayList(int initialCapacity)`:
初始指定长度的数组实体：`elementData = new Object[initalCaacity];`
2. `ArrayList()`:
初始默认长度的数组实体：`this.elementData = DEFAULTCAPACITY_EMPTY_ELEMENTDATA;`可见是长度为0的数组。
3. `ArrayList(Collection<? extends E> c)`:
用给定的集合初始化ArrayList

#### 扩容
对数组实体的扩容主要涉及到三个方法：
1. `ensureCapacityInternal`：
确定出需要扩容的大小。
2. `ensureExplicitCapacity`：
对扩容操作进行最后确定
3. `grow`：
执行扩容操作

#### ensureCapacityInternal 方法
```java
private void ensureCapacityInternal(int minCapacity) {
        if (elementData == DEFAULTCAPACITY_EMPTY_ELEMENTDATA) {
            minCapacity = Math.max(DEFAULT_CAPACITY, minCapacity);
        }

        ensureExplicitCapacity(minCapacity);
    }
```
根据上面add方法可知，形参minCapactiy的值为默认为`size+1`。
这里先判断elementData是否为空数组（通过Arraylist无参构造器创建，且为第一次调用），此时**确定需要扩容大小为`DEFAULT_CAPACITY`，其值为10。**

#### ensureExplicitCapacity 方法
```java
private void ensureExplicitCapacity(int minCapacity) {
        modCount++;

        // overflow-conscious code
        if (minCapacity - elementData.length > 0)
            grow(minCapacity);
    }
```
最终确认扩容操作，自增modCount，modCount用于记录对列表结构的修改次数，结构上的修改是指改变列表的大小，**在集合进行遍历的同时修改列表结构可能会使遍历结果产生错误，因此通过该值来指示（阻值）在遍历时的修改行为（以抛出ConcurrentModificationException异常的方式）。**

调用grow方法执行扩容操作，这部分代码虚拟机会向操作系统申请内存，如果没有申请到足够的内存，将有可能会触发 OutOfMemoryError 错误。

#### grow 方法（扩容策略）
```java
/**
     * Increases the capacity to ensure that it can hold at least the
     * number of elements specified by the minimum capacity argument.
     *
     * @param minCapacity the desired minimum capacity
     */
    private void grow(int minCapacity) {
        // overflow-conscious code
        int oldCapacity = elementData.length;
        int newCapacity = oldCapacity + (oldCapacity >> 1);
        if (newCapacity - minCapacity < 0)
            newCapacity = minCapacity;
        if (newCapacity - MAX_ARRAY_SIZE > 0)
            newCapacity = hugeCapacity(minCapacity);
        // minCapacity is usually close to size, so this is a win:
        elementData = Arrays.copyOf(elementData, newCapacity);
    }
```
grow方法对数组实体进行扩容，从而使ArrayList动态扩展集合大小，实现区别于数组的容量按需扩大能力。

扩容策略：
1. `oldCapacity + (oldCapacity >> 1)`一句是关键，它确定了新的数组容量：当前容量的3/2，即增加当前大小一半的数量。
2. 如果按规则计算出的新的容量仍然小于指定的容量，则按指定的容量扩容。
3. 如果通过以上两步计算出的容量大于了`MAX_ARRAY_SIZE`（其值为`Integer.MAX_VALUE - 8`），则执行`hugeCapacity`方法，调用了该方法则表示数组大小快要解决阀值，很有可能会触发OutOfMemoryError。

最后，在确定了新的容量之后，只需按新的大小创建新数组，将原数组中的元素复制到新数组即可。

### get 方法
```java
/**
     * Returns the element at the specified position in this list.
     *
     * @param  index index of the element to return
     * @return the element at the specified position in this list
     * @throws IndexOutOfBoundsException {@inheritDoc}
     */
    public E get(int index) {
        rangeCheck(index);

        return elementData(index);
    }

```
get方法用于从集合中获取指定下标的元素。
只需从数组实体中获取对应下标的元素即可，在此之前需要先判断数组下标是否越界。
