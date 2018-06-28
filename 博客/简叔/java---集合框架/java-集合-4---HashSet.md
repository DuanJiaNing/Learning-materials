## HashSet
参考文章：[HashMap和HashSet的区别](http://www.importnew.com/6931.html)

HashSet实现了Set接口，它不允许集合中有重复的值，当我们提到HashSet时，第一件事情就是在将对象存储在HashSet之前，要先确保对象重写equals()和hashCode()方法，这样才能比较对象的值是否相等，以确保set中没有储存相等的对象。如果我们没有重写这两个方法，将会使用这个方法的默认实现。

### 构造器
HashSet内部用于存储数据的实体为HashMap，构造器中会对HashMap实例map进行初始化。
```java
    private transient HashMap<E,Object> map;

    public HashSet() {
        map = new HashMap<>();
    }
    
    public HashSet(int initialCapacity, float loadFactor) {
        map = new HashMap<>(initialCapacity, loadFactor);
    }
    
    public HashSet(int initialCapacity) {
        map = new HashMap<>(initialCapacity);
    }
```

### add方法
```java

    // Dummy value to associate with an Object in the backing Map
    // 与后台映射中的对象关联的虚拟值
    private static final Object PRESENT = new Object();
 
    public boolean add(E e) {
        return map.put(e, PRESENT)==null;
    }
```
可见向HashSet中添加值时其实是将新值作为map的键存到HashMap中，而map所有键的值都为PRESENT，PRESENT是一个虚拟的值（无用的值）。

从这里就可以看出**HashSet保证元素唯一的方法是借助HashMap的键不重复**这一特性完成的。这也就解释了为什么存到HashSet中的元素要覆写其equals和hashCode方法（HashMap对于键的重复判定是通过hashCode和equals方法）。

### iterator
```java
    public Iterator<E> iterator() {
        return map.keySet().iterator();
    }
```
HashSet没有get方法来获得元素，只能通过迭代器或者toArray方法获得所有元素，而Iterator方法将获得map键集的迭代器。

此外可通过contains方法判断set中是否存入了目标元素。
