
**BLOG** 是学习 JavaWeb 开发4个月以来的总结实践项目，使用了SSM（Spring、SpringMVC、MyBatis）框架，MVC 三层结构，Shiro 身份，权限管理，Lucene 全文检索引擎工具的个人博文系统。

网站截图
![image](http://upload-images.jianshu.io/upload_images/7460499-c6de9ba49b785504..png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
GitHub地址：[DuanJiaNing/BlogSystem](https://github.com/DuanJiaNing/BlogSystem)

## 1. 开发环境
IDE：IntelliJ IDEA 2017.3 x64、Postman、navicat、Google Chrome
jdk版本：jdk1.8.0_45
MySQL版本：5.7.16-log
服务器：apache-tomcat-8.5.24

## 2. 项目结构
参考博文：[手把手教你整合最优雅SSM框架：SpringMVC + Spring + MyBatis](https://www.2cto.com/kf/201606/518341.html)

![image](http://upload-images.jianshu.io/upload_images/7460499-83d3c9df016a08c1..png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![image](http://upload-images.jianshu.io/upload_images/7460499-cf59398ba16f3393..png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

项目使用 SSM 框架，spring 的配置文件、mybatis 的映射文件、国际化资源、数据库文件以及项目的配置文件放置于 resources 目录中。

代码放置于 java 目录中，包分层包含 JavaWeb 项目基础包：entity、dao、dto、service、web，此外还有 util 工具包，枚举类包，异常类包、shiro 的 realm所在包，manager（类似于util包，但承当了比util更具针对性，与web.api耦合更紧密的一些功能）包。

#### 2.1 service 
业务层（service）根据用例参与者来进行划分，分为 audience（读者）、blogger（博主）、通用的 common 以及他们的实现（impl）。

#### 2.2 web
web 层分为 api 和 blog 两层，api 层主要为 url 接口，每一个 url 请求都返回 json 数据，用于前端 javaScript 访问，不涉及页面跳转控制。blog 对应一个具体的网站应用，即**BLOG**，**BLOG**使用了 api 包中提供的接口，同时依赖于其他的包。

#### 2.3 dao
dao 层定义了 MyBatis 的数据库操控接口，在 resources/mapper 目录中存放了其对应的映射文件。

#### 2.4 exception
exception 包中定义了所有的项目异常（错误信息），这些错误都是 RuntimeException，一些检查异常，如IOException也被转化为运行时异常，这样做使这些异常在繁杂的方法调用栈中可以不被吞噬，能够以简洁的方式抛到最上层，即 web 包中的类，由控制层（web层）决定如何处理这些异常，如对于 web.api 中抛出的错误信息，将会以 json 的方式将错误信息对应的代码以及错误的描述信息返回给API使用者。
参考了[聚合API](https://www.juhe.cn/docs/api/id/213)的设计。

## 3. 开发流程
项目开发流程参考[javaee项目开发流程](https://zhidao.baidu.com/question/433244940.html)，开发过程将一些过程省略（毕竟这是一个人的项目）。

将项目的开发分为两大部分：后端API接口开发和网站开发。

#### 3.1 后端 API 接口开发（web.api）
API开发的最终代码入口为 com.duan.blogos.web.api 包中的类，每一个类对应于一类数据的操控，不涉及页面跳转控制。api 的调用通过 url 的方式，url 的设计遵循 RESTful 风格。
参照：[RESTful API 设计指南](http://www.ruanyifeng.com/blog/2014/05/restful_api.html)

这部分的开发占据了大量时间，在最终编写 api 包中类的代码前需完成数据库表的设计，dao、entity、service 以及其他辅助类的编写以及测试，后续的网站开发也依赖于其中一部分类。

例如：`com.duan.blogos.web.api.BloggerLinkController#add` 方法
其对应的url：`/blogger/{bloggerId}/link`，bloggerId指博主id
请求方式：`POST`
返回数据类型：`JSON`
请求参数：`iconId`，`title`，`url`和`bewrite`。
代码为：
![Image_067.png](http://upload-images.jianshu.io/upload_images/7460499-69c3951b5928c8bd.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


对于 api 包中的每一个接口方法都有对应的文档说明。包含详尽的调用说明、返回参数说明以及错误代码说明。

详尽API文档见 [BlogSystem-wiki](https://github.com/DuanJiaNing/BlogSystem/wiki)

#### 1.2 网站开发 （web.blog）
网页开发的最终代码放置于 com.duan.blogos.web.blog 包中，这些类（或类中的方法）直接与前端页面对应，相互依赖，控制跳转和数据传递。

## 3. 用例图
#### 3.1 读者用例
![image](http://upload-images.jianshu.io/upload_images/7460499-0524c841160a60a3..jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### 3.2 博主用例
![image](http://upload-images.jianshu.io/upload_images/7460499-24ac296573fa5c6b..jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
