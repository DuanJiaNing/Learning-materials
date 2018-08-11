quartz 的分析见上一篇 [博文](https://www.jianshu.com/p/2456e3c527c0)

### 简介
xxl-job 是一个轻量级分布式任务调度平台，XXL-JOB 目前已接入多家公司的线上产品线，而且有完善的 [说明文档](http://www.xuxueli.com/xxl-job/#/?id=_13-%E5%8F%91%E5%B1%95)，相比 quartz  轻量，易学习，易扩展。

 作者在其 [文档](http://www.xuxueli.com/xxl-job/#/?id=_13-%E5%8F%91%E5%B1%95) 中对 xxl-job 的特性、如何入门、具体使用、配置以及架构都有完整的说明，在次简单列举几条特性：

> 1. 支持在可视化调度中心中动态修改任务状态、暂停/恢复任务，以及终止运行中任务，即时生效；
> 2.  注册中心: 执行器会周期性自动注册任务, 调度中心将会自动发现注册的任务并触发执行。同时，也支持手动录入执行器地址；
> 3.  失败处理策略；调度失败时的处理策略，策略包括：失败告警、失败重试；
> 4.  GLUE：提供Web IDE，支持在线开发任务逻辑代码，动态发布，实时编译生效，省略部署上线的过程。支持30个版本的历史版本回溯。
> 5.  调度中心HA（中心式）,执行器HA（分布式）

在 GitHub 可以下载到项目的源码，调度中心是一个 Spring MVC 项目，搭建 demo 前也有将项目完整 clone 到本地的必要。[/xuxueli/xxl-job](https://github.com/xuxueli/xxl-job/)

### XXL-JOB
![QQ截图20180701094517.png](https://upload-images.jianshu.io/upload_images/7460499-6bf9bfc987bf4645.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


#### 调度中心
> 统一管理任务调度平台上调度任务，负责触发调度执行，并且提供任务管理平台。
![QQ截图20180701094807.png](https://upload-images.jianshu.io/upload_images/7460499-1c3fe907f00a137d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


从 GitHub 拉下整个项目后，项目的目录结构如下：
![QQ截图20180701095109.png](https://upload-images.jianshu.io/upload_images/7460499-e68bb6911b13be29.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


调度中心为一个 Spring MVC 项目，需要进行部署运行（以单调度中心，单执行器为例），在部署前需要先完成以下步骤：

1.  导入数据库：运行 tables_xxl_job.sql 创建数据库
2.  配置调度中心：修改 xxl-job-admin 模块 resources 文件夹下 xxl-job-admin.properties 文件，主要为修改其中的 xxl.job.db.url、xxl.job.db.user 和 xxl.job.db.password 。告警邮箱，调度中心登录用户名和密码也可按需修改。
3.  完成后将项目打成 war 包部署到 tomcat
4.  浏览器网站访问即可

#### 执行器
> 负责接收“调度中心”的调度并执行；可直接部署执行器，也可以将执行器集成到现有业务项目中。

在 xxl-job-executor-sample 模块中作者提供了四种调度器，可以直接运行他们来进行测试，也可以自己另外新建项目测试，这里我选择自己另外新建 spring boot 项目的方式。

将 xxl-job 集成到 spring boot 项目中非常简单，按如下步骤操作即可：

1 添加依赖
```
  <!-- https://mvnrepository.com/artifact/com.xuxueli/xxl-job-core -->
        <dependency>
            <groupId>com.xuxueli</groupId>
            <artifactId>xxl-job-core</artifactId>
            <version>1.9.0</version>
        </dependency>

```

2  添加执行器配置
``` yml
xxl:
  job:
    executor: # 执行器配置
      appname: quartz-xxl-demo
      ip: 
      port: 9999
      logpath: /data/applogs/xxl-job/jobhandler
    accessToken:
    admin:
      addresses: http://127.0.0.1:5680 # xxl-job 調度器地址
```
配置文件我采用 yml 的方式，我的调度中心我部署在 5680 端口，因此这里调度器的地址为`http://127.0.0.1:5680` （注意：需要先将调度器启动），执行器需要单独的服务来处理调度器的调度，因此需要占用端口号，这里我设置的是 9999 端口。

3 开发示例任务
![QQ截图20180701123404.png](https://upload-images.jianshu.io/upload_images/7460499-d0b901f4d3cf7718.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


4 启动执行器项目
看到这些日志就表明 xxl-job 的执行器启动成功，而且连接到了调度中心。
![QQ截图20180701112051.png](https://upload-images.jianshu.io/upload_images/7460499-57a50419230819b9.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


5 在调度中心添加任务
![QQ截图20180701123611.png](https://upload-images.jianshu.io/upload_images/7460499-2f5760b13253100e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

添加后会根据 cron 规则进行调度，也可以手动触发执行。
![QQ截图20180701123806.png](https://upload-images.jianshu.io/upload_images/7460499-9134a777cff8a103.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
