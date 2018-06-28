原文地址：[Why String is immutable in Java?](https://www.programcreek.com/2013/04/why-string-is-immutable-in-java/)

### 为什么 String 在 java 中是不可变的？
String 在 java 中是不可变的，一个不可变类意味着它的实例在创建之后就不可修改，实例的所有属性在创建时初始化，之后无法对这些属性进行修改。不可变类型有着许多的优点，这篇文章总结了 **为什么 String 被设计成不可变的**，文章将从内存、同步和数据结构的角度说明不变性概念。

#### 1 字符串池的需要
字符串池是存在于 [Java方法区](http://www.cnblogs.com/wangguoning/p/6109377.html) 的一个特殊内存区域，当需要创建的目标字符串在字符串池中已经存在，那么字符串池中的字符串引用就会返回并赋值给目标字符串，而不是创建一个新的对象。

如下的代码只会在堆中创建一个对象：
```java
String string1 = "abc";
String string2 = "abc";
```
图形表示：
![](http://upload-images.jianshu.io/upload_images/7460499-e3d317f897a70739.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
如果 String 是可变的，一个引用改变字符串的值将会导致另一个引用在获取值时得到错误的值。（译者：另一个引用并未对字符串做修改，当他再次取值时字符串的值却与上次取的不同！）。

#### 2 用作缓存时的 hashcode
字符串的哈希值在 java 中是被频繁使用到的。举个例子，在 HashMap 或 HashSet 中，String 的不可变性保证了字符串 hashcode 的一致性，所以在进行缓存时无需担心字符串变化，这意味着，不需要在字符串每次被使用到时都计算其 hashcode 来保证一致性，这样更加高效。

在 String 类源码中有如下的代码：
```java
    /** Cache the hash code for the string */
    private int hash; // Default to 0
```
#### 3 便利其它对象的使用
具体化来说，考虑如下代码：
```java
HashSet<String> set = new HashSet<String>();
set.add(new String("a"));
set.add(new String("b"));
set.add(new String("c"));
for (String s : set){
    a.value = "a";
}
```
在这个例子中，如果 String 是可变的，那么它的 value 可以被改变，但这违反了 Set 的设计原则（Set 中的元素是不能重复的），当然，上面的例子仅仅为了表明意图，String 类中并不存在 value 成员。

#### 4 安全性
String 作为参数在 java 中广泛使用，例如网络连接，打开文件，等。如果 String 被设计为可变的，那么一个网络连接或者文件操作将会被改变，这可能留下非常严重的安全隐患。方法调用的预期结果是成功连接到设备，事实可能是并没有连接。可变的字符串在反射中也会导致安全问题，因为参数是字符串。

代码示例：
```java
boolean connect(String str){
    if (!isSecure(str)){
        throw new SecureException();
    }
    //在方法调用前如果 str 的值被其它引用对象改变，就会出现问题。
    caseProblem(str);
}
```

#### 5 不可变对象是天生线程安全的
因为不可变对象不会被改变，它们可以在多个线程间自由访问。这样就无需对存取进行同步。

总结，String 被设计为 final 的原因是 **效率** 和 **安全**，通常情况下这也是为什么不可变对象在许多设计中会成为首选的原因。

----------

##### 相关文章：
- [Create Java String Using ” ” or Constructor?](https://www.programcreek.com/2014/03/create-java-string-by-double-quotes-vs-by-constructor/)
- [Diagram to show Java String’s Immutability](https://www.programcreek.com/2009/02/diagram-to-show-java-strings-immutability/)
- [Top 10 questions of Java Strings](https://www.programcreek.com/2013/09/top-10-faqs-of-java-strings/)
- [LeetCode – Reverse Words in a String (Java)](https://www.programcreek.com/2014/02/leetcode-reverse-words-in-a-string-java/)
- [Java方法区](http://www.cnblogs.com/wangguoning/p/6109377.html)
