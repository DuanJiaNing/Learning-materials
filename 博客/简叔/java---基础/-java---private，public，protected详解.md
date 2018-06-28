![](http://upload-images.jianshu.io/upload_images/7460499-96cdf2e677497fd0.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
参照甲骨文的java文档：[点击打开链接](https://docs.oracle.com/javase/tutorial/java/javaOO/accesscontrol.html)
#### 访问级别
访问级别修饰符确定其他类是否可以使用特定字段或调用特定方法。有两个级别的访问控制：
###### 1.在顶级
 - public或package-private（没有显式修饰符即默认权限）。
类可以用修饰符public声明，在这种情况下，类对所有类都可见。如果一个类没有修饰符（默认，也称为package-private），它只在自己的包中可见。

###### 2.在成员级别 

- public，private，protected或package-private（没有显式修饰符即默认权限）。
在成员级别，也可以使用public修饰符或无修饰符（package-private），如同顶级类一样，具有相同的含义。
对于成员，除public和默认外有两个附加的访问修饰符：private和protected：
private修饰符指定该成员只能在其自己的类中访问。
protected修饰符指定该成员只能在其自己的包（如package-private）中访问，此外还可以由另一个包中的该类的子类访问。

下表显示了对每个修饰符允许的成员的访问权限。
![](http://upload-images.jianshu.io/upload_images/7460499-4d5714456c27a0bc?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)第一列指示类本身是否有权访问由访问级别定义的成员。正如你可以看到，一个类总是有权访问它自己的成员。
第二列指示与该类（不管其父级）相同的包中的类是否具有对成员的访问权限。
第三列指示在此包外部声明的该类的子类是否有权访问成员。
第四列指示是否所有类都具有对成员的访问权限。

#### 访问级别通过两种方式影响您
当您使用来自另一个源（例如Java平台中的类）的类时，访问级别将确定您自己的类可以使用的那些类的哪些成员
第二，当你写一个类时，你需要决定每个成员变量和类中的每个方法应该具有什么访问级别。

让我们看一下类的集合，看看访问级别如何影响可见性。下图显示了此示例中的四个类以及它们之间的关系。
![](http://upload-images.jianshu.io/upload_images/7460499-9bdc4f089154516a?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
此例中：
One包有Alpha和Beta两个类，Two包中有AlphaSub和Gamma类，其中AlphaSub继承Alpha
下表显示了Alpha类的成员（成员变量、成员函数）对于可应用于它们的每个访问修饰符的可见性。
![](http://upload-images.jianshu.io/upload_images/7460499-f4ba8a3f92f35af2?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
Alpha类内所有成员都可以访问
Beta与Alpha在同一包中，因而Alpha的私有成员Beta不能访问
Alphasub与Alpha不在同一包中，但AlphaSub是Alpha的子类，因而AlphaSub不能访问Alpha的默认和私有成员
Gamma与Alpha位于不同包且无继承关系，因而Gamma只能访问Alpha的public成员

#### 选择访问级别的提示：
如果其他程序员使用你的类，你想要确保不会发生滥用错误。访问级别可以帮助您这样做。
1.使用对特定成员有意义的最严格的访问级别。使用私人，除非你有一个很好的理由不使用。
2.避免使用公共字段（常量除外）。 （本教程中的许多示例使用公共字段。这可能有助于简要说明一些要点，但不推荐用于生产代码。）公共字段往往会将您链接到特定实现
并限制您在更改代码时的灵活性。

#### 文档看完了就实际编写例子验证一下吧（只验证了成员函数）：
其中每个类中的test方法是用来测试的
Alpha.java
分别定义了四个public，protected、默认，private权限的方法
```java
package com.one;  
  
public class Alpha  
{  
    public void test()  
    {  
        this.pub();  
        this.pro();  
        this.def();  
        this.pri();  
    }  
  
    public void pub()  
    {  
        System.out.println( "public");  
    }  
  
    protected void pro()  
    {  
        System.out.println( "protected");  
    }  
      
    void def()  
    {  
        System.out.println( "default");  
    }  
  
    private void pri()  
    {  
        System.out.println( "private");  
    }  
}  

```
Beta.java和Alpha在同一包下
```java
package com.one;  
  
public class Beta  
{  
    public void test()  
    {  
        Alpha alpha = new Alpha();  
        alpha.pub();  
        alpha.pro();  
        alpha.def();  
        alpha.pri();  
    }  
  
}  
```

AlphaSub.javaAlphaSub作为Alpha的子类，用super进行验证。
```java
package com.two;  
import com.one.Alpha;  
  
public class AlphaSub extends Alpha  
{  
    public void test()  
    {  
        super.pub();  
        super.pro();  
        super.def();  
        super.pri();  
    }  
}  
```
Gamma.java与Alpha不同包且无继承关系
```java
package com.two;  
import com.one.Alpha;  
  
public class Gamma  
{  
    public void test()  
    {  
        Alpha alpha = new Alpha();  
        alpha.pub();  
        alpha.pro();  
        alpha.def();  
        alpha.pri();  
    }  
}  
```

接下来就开始验证：
###### 1.验证Alpha类
```java
import com.one.Alpha;  
  
class  Test   
{  
    public static void main(String[] args)   
    {  
        new Alpha().test();  
    }  
}  
```
分别编译Alpha.java和Test.java：
 ```blash
F:\javaStuff>javac -d F:\javaStuff Alpha.java  
  
F:\javaStuff>javac Test.java  
  
F:\javaStuff>java Test  
public  
protected  
default  
private  
```
编译没有报错，即证明四种访问权限在类内都是可以访问的

###### 2.验证Beta类
依次编译Alpha.java和Beta.java
```blash
F:\javaStuff>javac -d F:\javaStuff Alpha.java  
  
F:\javaStuff>javac -d F:\javaStuff Beta.java  
Beta.java:11: 错误: pri()可以在Alpha中访问private  
                alpha.pri();  
                     ^  
1 个错误  
```
###### 3.验证AlphaSub类
依次编译Alpha.java和AlphaSub.java
```blash
F:\javaStuff>javac -d F:\javaStuff Alpha.java  
  
F:\javaStuff>javac -d F:\javaStuff AlphaSub.java  
AlphaSub.java:10: 错误: def()在Alpha中不是公共的; 无法从外部程序包中对其进行访问  
                super.def();  
                     ^  
AlphaSub.java:11: 错误: pri()可以在Alpha中访问private  
                super.pri();  
                     ^  
2 个错误  
```
Alpha的def方法为默认权限无法在外部包访问
Alpha的pri方法为私有权限无法在外部包访问
###### 4.验证Gamma类
依次编译Alpha.java和Gamma.java
```blash
F:\javaStuff>javac -d F:\javaStuff Alpha.java  
  
F:\javaStuff>javac -d F:\javaStuff Gamma.java  
Gamma.java:10: 错误: pro()可以在Alpha中访问protected  
                alpha.pro();  
                     ^  
Gamma.java:11: 错误: def()在Alpha中不是公共的; 无法从外部程序包中对其进行访问  
                alpha.def();  
                     ^  
Gamma.java:12: 错误: pri()可以在Alpha中访问private  
                alpha.pri();  
                     ^  
3 个错误  
```
Alpha的def方法为默认权限无法在外部包访问
Alpha的pri方法为私有权限无法在外部包访问
Alpha的pro方法为保护方法无法被外部非子类访问

代码的目录结构如下
![](http://upload-images.jianshu.io/upload_images/7460499-ae11ce29b986814b?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

因为编译Beta、AlphaSub和Gamma时都有编译错误自然就不会生成com.two包了

----
