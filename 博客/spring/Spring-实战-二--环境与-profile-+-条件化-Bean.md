## 环境与 profile
在 spring 3.1 版本中引入环境相关的 bean 功能，需要环境来决定该创建哪个 bean 和不创建哪个 bean，spring 在运行时才会做出决策， 即同一个部署单元（可能为 war 文件）能够适应用所有的环境，无需重新构建。

使用 profile 首先需将所有的 bean 定义整理到一个或多个 profile 中，部署时确保对应的 profile 激活。

### javaConfig 
在 javaConfig 中可以使用 @Profile 注解指明某个 bean 属于哪一个 profile，
@Profile 注解可用在类和方法上。
![QQ截图20180714090346.png](https://upload-images.jianshu.io/upload_images/7460499-e998b7234dda95f6.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

没有指定 profile 的 bean 都会被创建，与哪个 profile 激活无关。

### xml 中使用 profile
![QQ截图20180714090507.png](https://upload-images.jianshu.io/upload_images/7460499-7ac12212033e0d02.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 激活 profile
spring.profiles.active
spring.profiles.default
active 指明激活的 profile，如果 active 没有指定，则查找 default，激活 default 指定的 profile，如果 active 和 default 都没指定，那么就没有激活的 profile，只有那些没有指定 profile 的 bean 会被创建。

两个属性的设置途径：
1. 作为 DispatcherServlet 的初始化参数
2. Web 应用的上下文参数
3. JNDI 条目
4. 环境变量
5. JVM 的系统属性
6. 集成测试时，使用 @ActiveProfiles 注解配置

active 和 default 可以设置为多个值，中间用逗号隔开即可
![QQ截图20180714090629.png](https://upload-images.jianshu.io/upload_images/7460499-766fc65a8f685e55.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 条件化 Bean
@Conditional 注解，可以应用到带有 @Bean 注解的方法上，如果给定的条件为 true，Bean 才会被创建，否则不创建。
![图片1.png](https://upload-images.jianshu.io/upload_images/7460499-30099ddebd252bd4.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![图片2.png](https://upload-images.jianshu.io/upload_images/7460499-1f679f3099b6e214.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![图片3.png](https://upload-images.jianshu.io/upload_images/7460499-8fc82b12b59250dd.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![图片4.png](https://upload-images.jianshu.io/upload_images/7460499-43506a8795a07760.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![图片5.png](https://upload-images.jianshu.io/upload_images/7460499-255b20ec08b68927.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


