![](http://upload-images.jianshu.io/upload_images/7460499-8813638f17bcb3c8.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

java - final 关键字

### java - final 关键字

在 java 中，final 关键字可以修饰类，方法和变量。被 final 修饰通常指“这是无法改变的”。

#### 1. final 数据

可以分为两种情况：

1. 一个永不改变的编译时常量
2. 一个在运行时初始化的值，而你不希望改变它

##### 1.1 final 修饰基本数据类型

这类常量必须是基本数据类型，用 final  修饰，在定义时就必须赋值，对于编译期常量，编译器可以将该常量值代入任何用到它的计算式中，可以在编译期执行计算式，减轻运行时负担。

如：

```java
public final int i = 10; //定义时就赋值，基本数据类型
public final int[] is = {1,2,3}; //即 new int[]{1,2,3} 运行时才被确定，有 new 关键字，数组不是基本数据类型
public final String str = "str"; // String 不是基本数据类型
```

补充：java 中的基本数据类型，共 8 种：

- byte 1字节
- char 2字节
- short 2字节
- double 8字节
- int 4字节
- boolean 《 Java 虚拟机规范》描述为 4 个字节，boolean 数组为 1 个字节，具体还要看虚拟机实现
- float 4字节
- long 8字节  
  

##### 1.2 final 修饰引用数据类型

final 修饰引用数据类型时，final 关键字使被修饰的变量的引用不能改变，然而，对象自身却是可以修改的。

如：

```java
public class Test {
    public final Te te = new Te();// te 为引用数据类型，且被 final 修饰
    public void fun(){
//        te = new Te(); // 试图给 te 修改引用，指向一个新的对象，此时编译会报错
        te.i = 10; // te 的内容可以修改（对象自身可以修改）
    }
    class Te{
        int i = 3;
    }
}

```

当变量被定义为 final ，同时 也被 static 修饰，表示该变量被初始化之后就占据着一段不能改变的存储空间，而且只会有一份。

final 使变量不能修改引用（基本数据类型时为值不能修改），而 static 强调只有一份。

```java
public static final String = "str" // 所在类加载时会被初始化
```


##### 1.3 空白 final 

所谓空白 final 指被声明为 final，但又未给定初值的域。但编译器又需要确保在使用前变量被赋值，因而 java 要求空白 final 必须在构造器中赋值。静态空白 final 可在静态代码块中赋值，非静态空白  final 要在构造代码块或是每一个构造函数中赋值。

> 空白 final 为类使用者提供了更大的灵活性，在保证变量只被赋值一次的同时，可以根据对象而有所不同。

如：

```java
public class Test {

    private static final String STR = "str";
    private static final int A;
    private static final String STR1;

    static {  // 静态空白 final 只能在定义时赋值或在静态代码块中赋值
        A = 3;
        STR1 = "str1";
    }

    private final int b = 1;
    private final int c;
    private final String str;
    
    // 非静态空白 final 可在构造代码块中赋值
    {
        c = 1;
    }
    // 选择构造代码块赋值时只需在其中一个构造代码块中赋值即可
    {
    }

	// 选择构造函数赋值时必须在每一个构造函数中都进行赋值，因为编译器无法知道类使用者会选择哪一个构造函数构造对象
    public Test() {
        str = "str";
    }

    public Test(String s) {
        str = s;
    }
}
```


#### 2. final 参数

使用 final 修饰方法参数意味着无法在方法中修改参数的值或引用。这一特性主要用于向匿名内部类传递参数。

方法中的匿名内部类如果要直接使用外部方法的参数（或是局部变量），必须将参数或是局部变量声明为 final 。

原因：

1. 当编译器将 java 文件编译为 class 文件时，匿名内部类和其所在的类生成的 class 文件并不是一份，而是两份（这类似于编译器对内部类的处理），用一份单独的 class 文件保存匿名内部类，然而匿名内部类要用到另一份 class 文件中的变量，他俩又是两份独立的 class 文件，此时只能**将用到的变量作为匿名内部类构造函数的参数传到匿名内部类**中（使用匿名内部类时是无法添加构造函数的），匿名内部类会生成同名的域来保存这些参数。这个工作是由编译器完成的。
<br>
2. 为什么要用 final 修饰，上面说了编译后是由单独的一份 class 文件保存匿名内部类，然而我们在编写匿名内部类的时候程序“看起来”是直接使用了变量的（没有传参，复制引用的过程），如果匿名内部类修改了变量，那外部的变量也应该相应的做出改变，外部修改了变量那匿名内部类中也应该改变，或者说匿名内部类跟外部的方法是一体的，他们读取到变量的值应该是一样的，如果不要求使用 final 修饰，那外部修改了变量的值，匿名内部类读取时读取到的值却是初始时传入的值，这时就不一致了，这是不符合预期的，我们使用匿名内部类的初衷大多时候就是能直接使用外部的变量，使内外在某一属性上保持一致，因此使用 final 修饰就能保证唯一性，达到一致的效果。


如：

```java
public class Test {
    public void test(final String str, final int id) {
        final String str2 = "";
        new In() {
            @Override
            public void fun() {
//                id += 2; // 不能修改 编译无法通过
                if (str.contains("a")) {
                    str.replace("a", "b"); // 引用不能修改，但变量本身可以修改，此时内外保持一致
                }
                int len = str2.length();
            }
        };
    }

    interface In {
        void fun();
    }
}
```

参考文章：[java提高篇(十)-----详解匿名内部类 ,形参为什么要用final](http://www.cnblogs.com/xiaorenwu702/p/5167997.html)


#### 3. final 方法

使用 final 修饰方法的原因有两个：

1. 把方法锁定，以防任何继承类修改它的定义，这是出于设计考虑，确保在继承中使方法行为保持不变，并且不会被覆盖
2. 过去使用 final 修饰方法的第二个原因是效率，在 java 的早期实现中，将一个方法指明为 final 后，编译器将针对该方法的所有调用转为内嵌调用，这将在一定程度上消除方法调用的开销，但当方法体积很大时，程序代码就会膨胀，这将导致内嵌调用无法提高效率。在最近的 java 版本中，虚拟机可以探测到这些情况，并去掉这些效率反而降低的内嵌调用，因此**不再需要使用 final 方法进行优化了**


> 只有在想明确禁止覆盖时，才应该使用 final 修饰方法


#### 4. final 和 private 关键字

类中所有的 private 方法都隐式的指定为 final 的、，由于无法取用 private 方法，所以也就无法覆盖它。可以对 private 方法添加 final 关键字修饰，但这并不能给方法增添任何额外的意义。

“覆盖”只有在某方法是基类的接口的一部分时才出现，private 方法不属于类的接口的一部分，所以也就没有覆盖的说法，可以在子类中定义与父类中 private 方法相同签名的方法，但他们两者仅仅是“签名相同”而已，此外没有任何关联。


#### 5. final 类

将某个类整体定义为 final ，就表明你不打算继承该类，而且也不允许别人继承。

final 类的域可以根据意愿选择是否为 final，final 关键字对于域的作用不受其所在类是否为 final 影响。

由于 final 禁止继承，所以 final 类中所有的方法隐式指定为 final ，因为无法覆盖他们，在 final 类中可以给方法添加 final 关键字，但这不会增添任何意义。

final 关键字修饰类时表明类无法继承，这意味着类的功能仅限于此，同时也不允许自己和别人对该类进行扩展，有时这是明智的，但要预见类是如何被复用的一般是很困难的，特别是对于一个通用的类更是如此，因此在决定将一个类或方法声明为 final 时，请思考其功能是否仅限于此，是否真的没有任何可能被继承或以其它方式被扩展。


--------------

文章大部分摘抄自《Java 编程思想》 第七章 7.8节 final 关键字，加上部分自己的理解，若有错误，欢迎指正。

----------
