## TCP和UDP
TCP/IP协议是一个协议族。里面包括很多的协议。UDP只是其中的一个。之所以命名为TCP/IP协议，因为TCP/IP协议是两个很重要的协议，就用他两命名了。

在TCP/IP模型中，<b>UDP和TCP为网络层以上和应用层以下提供接口。</b>
<p><b>TCP/IP协议族四层模型</b></p>
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-69c35d674b7e339d?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 1. TCP
参考文章：[传输控制协议](https://zh.wikipedia.org/wiki/%E4%BC%A0%E8%BE%93%E6%8E%A7%E5%88%B6%E5%8D%8F%E8%AE%AE )
Transmission Control Protocol，传输控制协议，是一种<b>面向连接的、可靠的、基于字节流的传输层通信协议。</b>
<p>TCP协议的运行可划分为三个阶段：连接创建、数据传送、连接终止。

应用层向TCP层（传输层）发送用于网间传输的、用8位字节（一个字符）表示的数据流，然后TCP把数据流分区成适当长度的报文段。之后TCP把结果包传给IP层（网络互联层），由它来通过网络将包传送给接收端实体的TCP层。

TCP为了保证不发生丢包，<u>就给每个包一个序号</u>，同时序号也保证了传送到接收端实体的包的**按序接收**。然后接收端实体对已成功收到的包发回一个相应的确认（ACK）；如果发送端实体在合理的往返时延（RTT）内未收到确认，那么对应的数据包就被假设为已丢失将会被进行**丢失（发送端）重传**。TCP用一个**校验和函数**来检验数据是否有错误；在发送和接收时都要计算校验和。

#### 1.1 TCP的三次握手建立连接
简写解释:

-	SYN表示建立连接
-	FIN表示关闭连接
-	ACK表示响应
-	PSH表示有DATA数据传输
-	RST表示连接重置

**三次握手**：
1.	客户端向服务器发送一个SYN J （客户端请求）
2.	服务器向客户端响应一个SYN K，并对SYN J进行确认ACK J+1（服务端响应请求）
3.	客户端再想服务器发一个确认ACK K+1（客户端确认收到服务端响应）

对应于Socket编程的几个方法的调用顺序为：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-46c76b01c17c0f3d?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
<p>当客户端调用connect时，触发了连接请求，向服务器发送了SYN J包，这时connect进入阻塞状态；服务器监听到连接请求，即收到SYN J包，调用accept函数接收请求并向客户端发送SYN K（连接请求）和ACK J+1（响应），这时accept进入阻塞状态；客户端收到服务器的SYN K ，ACK J+1后connect方法返回，客户端发送ACK K+1进行最终确认；服务端收到ACK K+1后accept返回，至此三次握手完毕，连接建立。</p>
**客syn服 -> 服syn+ack客 –> 客ack服**
#### 1.2 TCP的四次握手释放连接
**四次挥手：**
1.	某个应用进程首先调用close主动关闭连接，这时TCP发送一个FIN M；
2.	另一端接收到FIN M之后，对这个FIN进行确认，同时将执行被动关闭；
3.	一段时间之后，接收到文件结束符的应用进程调用close关闭它的socket。这导致它的TCP也发送一个FIN N；
4.	接收到这个FIN的源发送端TCP对它进行确认。
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-41905f45785d8d65?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
**客fin服务 -> 服ack客 -> 服fin客 -> 客ack服务**
### 2.UDP
参考文章：[用户数据报协议](https://zh.wikipedia.org/wiki/%E7%94%A8%E6%88%B7%E6%95%B0%E6%8D%AE%E6%8A%A5%E5%8D%8F%E8%AE%AE )

User Datagram Protocol，缩写为UDP，又称用户数据报文协议，是一个**简单的面向数据报的传输层协议**。

UDP只提供数据的**不可靠传递**，它<u>一旦把应用程序发给网络层的数据发送出去，就不保留数据备份</u>（所以UDP有时候也被认为是不可靠的数据报协议）。

UDP在IP数据报的头部仅仅加入了复用和数据校验（字段）。由于缺乏可靠性且属于非连接导向协议，UDP应用一般必须允许一定量的丢包、出错。流媒体（流技术）、即时多媒体游戏和IP电话（VoIP）一定就是典型的UDP应用。如果某个应用需要很高的可靠性，那么可以用传输控制协议（TCP协议）来代替UDP。
### 3. 两者区别
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-84216c08b3d02cf8?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
