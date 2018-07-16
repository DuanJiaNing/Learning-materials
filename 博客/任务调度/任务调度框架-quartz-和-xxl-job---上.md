
### 1 比较

quartz 和 xxl-job 都是任务调度框架，任务调度相关功能都可以借助这两个框架实现。
任务调度指在将来某个特定的时间、固定的时间周期或规律变化的时间周期到达时自动调度并执行指定的任务。

这两个框架都通过自己的方式实现了这个功能，区别在于对任务的创建、修改、删除、触发以及监控的操作成本上，quartz 对于这些操作直接提供了 api，这意味着开发人员拥有最大的操作权，也带来了更高的灵活性，然而对于不需要很高灵活性的系统，或调度任务的操控由非开发人员负责的系统，需要额外对 api 调用做一层封装，隔离 api 操作；而 xxl-job 提供这些控制的方式是提供了一个单独的可视化调度中心，这意味着任务的状态控制可以和系统分离，通过更易操作的网页界面的形式，降低了对操控者的门槛。

### 2 quartz 
![QQ截图20180630102007.png](https://upload-images.jianshu.io/upload_images/7460499-a1adf923430c6ac6.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### 2.1 概念

quartz 有四个重要的组件，使用前需要先了解他们。
参考：[Quartz 入门详解](http://www.importnew.com/22890.html)

**1. Scheduler**
代表一个 Quartz 的独立运行容器，Scheduler 将 Trigger 绑定到特定 JobDetail， 这样当 Trigger 触发时， 对应的 Job 就会被调度。

**2. Trigger**
描述 Job 执行的时间触发规则。主要有 SimpleTrigger 和 CronTrigger 两个子类，通过一个 TriggerKey 唯一标识。

**3. Job**
定义一个任务，规定了任务是执行时的行为。JobExecutionContext 提供了调度器的上下文信息，Job 的数据可从 JobDataMap 中获取。

**3. JobDetail**
Quartz 在每次执行 Job 时，都重新创建一个 Job 实例，所以它不直接接受一个 Job 的实例，相反它接收一个 Job 实现类。描述 Job 的实现类及其它相关的静态信息，如 Job 名字、描述等。

#### 2.2 demo

##### 2.2.1 封装

添加一个任务到 quartz 容器中
![QQ截图20180630095545.png](https://upload-images.jianshu.io/upload_images/7460499-dabbd9b4c0b12de0.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

ScheduleJob
![QQ截图20180630095424.png](https://upload-images.jianshu.io/upload_images/7460499-a4833f4e9a92e802.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

暂停、恢复、删除一个任务只需调用 Scheduler 的对应方法即可。
```
JobKey jobKey = JobKey.jobKey("jobName", "jobGroup");
scheduler.pauseJob(jobKey);
scheduler.resumeJob(jobKey);
scheduler.deleteJob(jobKey);
scheduler.triggerJob(jobKey);
```
CronTrigger 相比 SimpleTrigger  提供了更灵活的调度规则，cron 表达式的知识可参考 [cron 表达式详解](https://www.cnblogs.com/javahr/p/8318728.html)。
[在线 cron 表达式生成器](http://cron.qqe2.com/)

##### 2.2.2 使用

**Job 实现类**
![QQ截图20180630100341.png](https://upload-images.jianshu.io/upload_images/7460499-713fc6a4dc68519b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


tip：Job 实现类需要用 public 修饰，即不能是内部类。

**InitializingBean#afterPropertiesSet** 中启动调度器
![QQ截图20180630100423.png](https://upload-images.jianshu.io/upload_images/7460499-49430ae18fa77b5a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


**调度后的控制台日志**
```
2018-06-30 10:02:30.013  INFO 1872 --- [eduler_Worker-1] com.duan.quartzdemo.quart.JobOne         : job-1 is schedule
2018-06-30 10:02:33.002  INFO 1872 --- [eduler_Worker-2] com.duan.quartzdemo.quart.JobOne         : job-1 is schedule
2018-06-30 10:02:36.001  INFO 1872 --- [eduler_Worker-3] com.duan.quartzdemo.quart.JobOne         : job-1 is schedule
```
#### 2.3 其他
demo 为 spring boot 项目，已上传 GitHub ，你可以在 [这里](https://github.com/DuanJiaNing/demos/tree/master/quartz-demo) 找到。

jdk 1.8
spring boot 集成了 quartz
```
        <dependency>
            <groupId>org.springframework.boot</groupId>
            <artifactId>spring-boot-starter-quartz</artifactId>
        </dependency>
```
使用 了 lombok 框架的以下注解：
```java
@Slf4j
@Data
@NoArgsConstructor
@AllArgsConstructor
```

```

        <dependency>
            <groupId>org.projectlombok</groupId>
            <artifactId>lombok</artifactId>
            <optional>true</optional>
        </dependency>
```


-------------
xxl-job 的分析见下篇 [博文](https://www.jianshu.com/p/6d452ababa3e)
