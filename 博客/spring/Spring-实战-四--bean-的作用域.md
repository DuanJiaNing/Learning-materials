在默认的情况下，Spring 上下文的所有 bean 都是作为单例存在的。
单例会保持一些状态，对象会被污染，有时重用时不安全的。

作用域：
1. 单例（Singleton）：整个应用中，只创建 bean 的一个实例
2. 原型（Prototype）：每次注入或通过 Spring 上下文获取时都创建新的实例
3. 会话（Session）：在 Web 应用中，为每个会话创建一个 bean 实例
4. 请求（Request）：在 Web 应用中，为每个请求创建一个 bean 实例

使用 @Scop 组合 @Bean 和 @Component（包括自注解，被 @Component 注解的注解）。
![image.png](https://upload-images.jianshu.io/upload_images/7460499-8430c881bfff7716.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image.png](https://upload-images.jianshu.io/upload_images/7460499-c4c9f545d4a266c1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
在 xml 中，bean 标签有对应的 socp 属性。

## 会话和请求作用域
就购物车功能而言，会话作用域最为合适。
![image.png](https://upload-images.jianshu.io/upload_images/7460499-a68518ffb4575e94.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
SCOPE_SESSION 对于一次会话会创建新的实例。
proxyMode 解决了将会话或请求作用域 bean 注入到单例 bean 中的问题。

实际注入时会注入目标 bean 的一个代理，使用时代理会对其进行懒解析并将调用委托给会话作用域内正真的 bean。

ScopedProxyMode.INTERFACE 指明目标类是一个接口，注入时需使用 JDK 动态代理
ScopedProxyMode.TARGET_CLASS 指明目标类是类，注入时使用 CGLib 代理
![image.png](https://upload-images.jianshu.io/upload_images/7460499-392697d35bbf8a42.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
xml 中也提供了相应属性：bean 元素的 scop 属性，以及 bean 元素下的子元素 aop:scoped-proxy 。
<aop:scoped-proxy proxy-target-class="false"/>
通过 proxy-target-class 属性指明目标 bean 是接口还是类。
