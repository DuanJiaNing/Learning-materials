###  简介

1. 中间件，提供协调服务
2. 作用于分布式系统，发挥其优势，可以为大数据服务
3. 支持 java 语言，提供 java 和 c 的客户端 api

### 特性

1. 一致性：数据一致性，数据按顺序分批入库
2. 原子性：事务要么成功要么失败，不会局部化
3. 单一视图：客户端连接集群中任一 zk 节点，数据都是一致的
4. 可靠性：每次对 zk 的操作状态会保存在服务端
5. 实时性：客户端可以读取到 zk 服务端的最新数据

### zoo.cfg 配置

1. tickTime: 用于计算的事件单元。比如 session 超时：N * tickTime
2. initLimit: 用于集群，允许从节点连接并同步到 master 节点的初始化时间，以 tickTime 的倍数
3. syncLimit: 用于集群，master 主节点与从节点之间发送消息，请求和应答的时间长度（心跳机制）
4. dataDir: 必须配置，数据存放目录
5. dataLogDir: 日志目录，不配置则与 dataDir 相同
6. clientPort: 连接 zookeeper 服务器的端口，默认 2181

安装版本 3.4.12 
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-825ceacceabeb190?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

zoo.cfg 为默认的配置文件，复制一份 zoo_sample.cfg，在复制的文件上进行修改。
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-61365d3c4fbda0a1?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

环境变量
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-d50172c03deefb36?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 常用命令

1. 启动服务
./zkServer.sh {**start**|start-foreground|stop|restart|status|upgrade|print-cmd}
2. 连接到zookeeper
bin/zkCli.sh -server 127.0.0.1:2181
3. 查看跟目录下的所有 Znode
ls /
4. 创建新节点，数据为“my_data”
create /zk_test my_data
5. 获取节点数据
get /zk_test
6. 修改节点数据为 junk
set /zk_test junk
7. 删除节点
delete /zk_test
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-1d220ee4547f4ab0?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
### 基本数据模型
 
1. 可理解为 linux/unix 的文件目录
2. 每个节点都称之为 znode，它可以有子节点，也可以有数据
3. 节点分为临时节点和永久节点，临时节点在客户端断开后消失
4. 每个 zk 节点有各自的版本号，可以通过命令查看节点信息
5. 每当节点数据发生变化，那么该节点的版本号会累加（乐观锁）
6. 删除/修改过时节点，版本号不匹配会报错
7. 每个 zk 节点存储的数据不宜过大，几 K 即可
8. 节点可以设置权限 acl，可以通过权限限制用户的访问


### 分布式系统

1. 很多台计算机组成一个整体，一个整体一致对外并且处理同一请求
2. 内部每台计算机都可以相互通信（rest/rpc）
3. 客户端到服务端的一次请求到响应结束会历经多台计算机

使用多台计算机，将一个系统拆分成多台子系统，根据业务进行拆分，服务化，从而使请求量、负载能力得到提高，即微服务。
 
### zk 作用

1. master 节点选举，主节点宕机之后，从节点就会接手工作，并且保证这个节点是唯一的，这就是所谓首脑模式，从而保证集群的高可用
2. 同一配置文件管理，即只需要部署一台服务器，就可以把相同的配置文件同步更新到其他所有服务器，此操作在云计算中用得特别多（比如修改了 redis 的配置）
3. 发布与订阅，类似消息队列 MQ（amp,rmp ...)，dubbo发布者把数据存到 znode 上，订阅者会读取这个数据。
4. 提供分布式锁，分布式环境中不同进程间争夺资源，类似多线程中的锁。
5. 集群管理，集群中保证数据的强一致性。





