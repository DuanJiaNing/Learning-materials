
## RocketMQ 入门

what is rocketMQ：RocketMQ 是阿里开源的一款高性能、高吞吐量的分布式消息中间件。使用 RocketMQ 可以建立可靠的同步，异步和单向的消息传送。

RocketMQ 概念学习参看博文：[十分钟入门RocketMQ](http://jm.taobao.org/2017/01/12/rocketmq-quick-start-in-10-minutes/)，以及 [官方文档](https://rocketmq.apache.org/docs/rmq-arc/)。

### rocketMQ 架构图：

![image](http://upload-images.jianshu.io/upload_images/7460499-254b6fa11d8f255a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 实践
环境搭建可参考：[linux下搭建rocketmq](http://www.bubuko.com/infodetail-2056221.html)

#### 在 linux 系统中启动

重点说下 broker 的配置问题和启动，假设 name server 和 broker 都部署在同一台服务器上，broker 采用单 master 模式。假设服务器 IP 为 *.*.*.*，我的 broker 的配置文件如下：

../../target/apache-rocketmq/conf
broker-test.conf
```
brokerClusterName = duan-cluster-test
brokerName = broker-test
brokerId = 0

brokerPort=10911

# 多网卡时指定 IP
brokerIP1=*.*.*.*

# NameServer地址列表，多个nameServer地址用分号隔开
namesrvAddr=*.*.*.*:9876

storePathRootDir = /data/rocketmq/store
storePathCommitLog = /data/rocketmq/store/commitlog
autoCreateTopicEnable = true
autoCreateSubscriptionGroup = true

deleteWhen = 04
fileReservedTime = 48
brokerRole = ASYNC_MASTER
flushDiskType = ASYNC_FLUSH
```
配置好后先启动 name server，后启动 broker，注意在 broker 启动命令中指定配置文件：
`nohup bin/mqbroker -c ./conf/broker-test.conf &> 
/data/rocketmq/logs/mqbroker.log &`

成功启动后再日志中可看到 success 字样。
`tail -f ~/logs/rocketmqlogs/broker.log`
`tail -f ~/logs/rocketmqlogs/namesrv.log`

除了 broker 和 master 外 还会有一个 HAserver，该服务用于同步 master 和 slave 间 的数据。

#### spring-boot 项目学习 api 的简单使用
maven 依赖如下
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-fa9d25632c4fef63?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

##### 测试用的配置文件
配置文件采用 yml 格式
```
rocketmq:
  nameSrv:
    address: *.*.*.*:9876
  producer:
    group:
      test: rkmq-pro-group-one
  consumer:
    group:
      test: rkmq-con-group-test
  topic:
    test: rkmq-topic-test
  tag:
    test: rkmq-tag-test
```
##### 编写代码
父类
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-5fc1104a20e93893?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

###### 消息生产者
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-66a2220b8eea3dc8?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

###### 消息消费者
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-961d0ab8473e3cd7?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

###### 消息消费回调
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-9a58dbd50800e010?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

##### 测试
用来测试的 bean
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-1d93b7cdfe235ae3?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

controller
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-12e1e39fe42c4f07?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

通过 url 传递不同的参数来测试。

测试 url：http://localhost:8080/3?obj=数据来了

控制台输出：
```
数据来了 0
数据来了 1
msg received 数据来了
数据来了 2
msg received 数据来了
msg received 数据来了
```

rocketMQ-console：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-da1142f945ebc903?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-d1e8a08fe9a17aa7?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-cec598af465e01a6?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-37840913afd27331?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

可视化控制台可在这里获得 [rocketMQ-console](https://github.com/apache/rocketmq-externals/blob/master/rocketmq-console/doc/1_0_0/UserGuide_CN.md)
