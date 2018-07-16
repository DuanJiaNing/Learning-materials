自动装配仅在只有一个 bean 匹配时有效，如果不只有一个 bean 的话，就会阻碍 spring 自 动装配属性、构造器参数或方法参数。

解决：
1. 将可选 bean 中的每一个设为首选（ primary ）
2. 使用限定符来帮助 spring 缩小范围（ qualifier ）

### 标识首选的 bean
@Primary 可与 @Component 和 @Bean 注解配合使用。xml 中对于 bean 标签有对应的 boolean 属性 primary。
![image.png](https://upload-images.jianshu.io/upload_images/7460499-ff3a50bdcdec1699.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-ceb34b99331a6562.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-1b09ef7d8474d878.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
当有两个可选的 Dessert 时，spring 无法从多个首选 bean 中做出选择，因此 @Primary 只在唯一时有效。

### 明确限定 bean
@Qualifier 注解是使用限定符的主要方式，可以与 @Autoware 和 @Inject 注解配合使用。
![image.png](https://upload-images.jianshu.io/upload_images/7460499-384a8d0f83868541.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
缺点：@Qualifier 指定的 bean id 与类名（方法名）紧耦合，类名修改时 @Qualifer 注解将失效。

通过在 Bean 定义处和使用处同时使用 @Qualifier 注解，可以实现基于 String 描述的对多个限定特性进行限定（java 8 才支持重复注解，java 8 允许在一个条目上重复使用一个注解，只要这个注解本身有 @Repeatable 注解。但 @Qualifier 不支持重复注解），因此只能明确限定一个特性。
![image.png](https://upload-images.jianshu.io/upload_images/7460499-3146339889214019.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-44a400dd05f8e8c7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

可以通过创建自定义的注解来实现限定符的功能，这些注解需要被 @Qualifier 注解注解，那么这个注解本身可以作为一个限定特性。
![image.png](https://upload-images.jianshu.io/upload_images/7460499-198c102310d9c16d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-a734332950fcb488.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-0267613d2165d68c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-cf85f01662851f49.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-ec540966c63ff96d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
原始的 @Qualifier 注解使用 String 进行限定，而基于 @Qualifier 注解的注解使用注解进行限定，类型更为安全，同时能够解耦。
