## 常用命令
启动 zookeeper: ./zkServer.sh start
连接本地 zookeeper：./zkCli.sh

1. ls 与 ls2：ls 只看节点，ls2 可以查看节点状态
![QQ截图20180713083619.png](https://upload-images.jianshu.io/upload_images/7460499-dba21c6ec09c015a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
2. stat: 查看状态（ls2 = ls + stat）
3. get: 获取节点数据
4. create: create [-s] [-e] path data acl 创建节点
创建默认节点，非顺序，持久化的
create /imooc imooc-data
创建临时节点
create -e /imooc/tmp imooc-tmp-data
创建顺序节点
create -s /imooc/sec seq
![QQ截图20180713083641.png](https://upload-images.jianshu.io/upload_images/7460499-e1aed97380eff02e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
5. set path data [version]
![QQ截图20180713083911.png](https://upload-images.jianshu.io/upload_images/7460499-1a6f06ca2c778834.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
版本号要跟节点数据的版本号一致，否则无法更新
6. delete path [version]
![QQ截图20180713083936.png](https://upload-images.jianshu.io/upload_images/7460499-8072249692b843ec.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 节点状态解读
![QQ截图20180713084021.png](https://upload-images.jianshu.io/upload_images/7460499-ba413557818bd2ba.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

cZxid: 创建后 zk 为当前节点分配的 id
ctime: create time，节点创建时间
mZxid: 节点修改后的 id
mtime: 节点修改时间
pZxid: 子节点 id
cversion: 子节点 version，子节点数据变化时 cversion 会变化
dataversion: 当前节点的数据版本号，当当前节点数据修改后，版本号会加一
aclVersion: 访问控制列表版本，权限变化时累加
ephemeralOwner：短暂的拥有者，临时节点绑定到的 session id
dataLength：数据长度
numChildren: 子节点数

## 特性 - session
1. 客户端与服务端的连接为一次会话
2. 每个会话都会设置一个超时时间
3. 心跳结束，session 过期（连接过程中，客户端会定期向服务端发送心跳指令，表明自己还存活）
4. session 过期，则临时节点 znode 会被删除
5. 心跳机制：客户端向服务端的 ping 包请求
6. watcher 机制
