spring 提供了两种在运行时求值的方式：

1. 属性占位符（Property placeholder）
2. spring 表达式语言（SpEL）

![image.png](https://upload-images.jianshu.io/upload_images/7460499-4e5e5d71b98e74f5.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-00c29c3d8741c1cd.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 属性占位符
形式为 "${...}" ，使用 @Value 注解，java 配置需要配置 PropertySourcesPlaceholderConfigurer bean，xml 可通过 <context:property-placeholder /> 开启。
![image.png](https://upload-images.jianshu.io/upload_images/7460499-997d7b3e8a85aff2.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-5e1e757589437965.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

xml 中也可通过 “${...}” 的形式使用。

### 使用 spring 表达式语言装配

1. 使用 bean id 引用其他 bean
2. 调用方法和访问对象属性
3. 对值进行算术、关系和逻辑运算
4. 正则表达式匹配
5. 集合操作

SpEL 要放到 “#{...}” 表达式中，在 xml 中也可以使用。

#### #{T(System).currentTimeMillis()}
T()表达式用于将括号内表达式视为 java 类，能够访问目标类型的静态方法和常量。也直接装配目标类型。

#### #{envOne.version}
envOne 为 bean 的 id，可获得其可访问成员的值

#### #{systemProperties['jnidispatch.path']}
systemProperties获取系统属性

#### #{envOne.getClass().toString().toUpperCase()}
调用 bean 的方法

#### #{envOne.getStr(true)?.toUpperCase()}
envOne.getStr(true) 返回 null 就不调用 toUpperCase 方法

#### #{T(java.lang.Math).random()}
调用 java 类的静态方法
![image.png](https://upload-images.jianshu.io/upload_images/7460499-065b1f8fabdb7737.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### 字面值
 \#{3.1415926}
\#{'Hello'}
\#{false}
\#{9.87E4}
![image.png](https://upload-images.jianshu.io/upload_images/7460499-5f0cfdb03d2b906a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### 运算符
算术：+  -  *  /  %  ^
比较：<  >  ==  <=  >=  lt  gt  eq  le  ge
逻辑：and  or  not  |
条件：?: (ternary)  ?: (Elvis)  三元运算符，Elvis 表达式
正则：matches
![image.png](https://upload-images.jianshu.io/upload_images/7460499-615482e3e11be74f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-8879f2eacab9189d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### 集合
![image.png](https://upload-images.jianshu.io/upload_images/7460499-70eb09960cbde9c6.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-999be5aff78b9c17.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-c69641ae0fc5bdd4.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 测试
![image.png](https://upload-images.jianshu.io/upload_images/7460499-b2200cfe6279d2c2.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-3702179f703af042.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
