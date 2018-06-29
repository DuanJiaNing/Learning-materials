1.Android并不是Google创造的，而是Android公司创造的，该公司的创始人是Andy Rubin。该公司后来被Google收购，而Andy Rubin成为Google公司的Android产品负责人。

2.Google于2007年11月5日发布了Android 1.0手机操作系统。于2009年5月发布了Android 1.5版本，该版本提供了一个非常“豪华”的用户界面，提供了蓝牙连接支持，吸引了大量开发者的目光。

3.Android已经超出了手机操作系统的范畴，Android系统已经广泛应用于TV，手表及各种课穿戴设备。

4.从2008年9月22日，T-Mobile在纽约正式发布第一款android手机——T-Mobile G1开始，Android系统不断获得各个手机厂商的青睐。

5.Android 5.x平台架构及特性：
![](http://upload-images.jianshu.io/upload_images/7460499-5a45e61edc8bf252?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
由图可看出Android系统主要由5部分构成
（1）应用程序层：
Android系统将会包含一系列的核心应用程序，包括sms程序、日历、浏览器、联系人等。这些应用程序通常由java编写。

（2）应用程序框架
当开发人员开发Android应用程序时就是面向底层的引用程序框架进行的。从这个意义上来看，android上的应用程序是完全平等的，不管是Android系统提供的程序还是普通开发者提供的程序，都可以访问Android提供的API框架。

（3）函数库
Android包含一套被不同组件使用的c/c++库的集合，一般来说，Android开发人员不能直接调用这套库，但可以通过它上面的引用程序框架来调用这些库。
下面列出一些核心库：
系统c库：标准c系统库，并且专门为嵌入式linux系统设备调整过
媒体库：这套媒体库支持播放和录制许多流行的音频和视频格式
Surface Manager：管理对现实子系统的访问，并可以对多个应用程序的2D和3D图层进行无缝整合
LibWebCore：Web浏览器引擎，为WebView提供支持
SGL：底层的2D图层引擎
3DLibraries：实现3D系统，可使用硬件3D加速，也可使用高度优化的软件3D加速
FreeType：位图和向量字体显示
SQLite：供所有应用程序使用的功能强大的轻量级关系数据库

（4）Android 运行时
Andriod运行时由两部分组成：Android核心库集和ART。其中核心库集提供了java语言核心库所能使用的绝大部分功能，而虚拟机则负责运行Android应用程序。
Android 5.0之前的Android运行时由Dalvik虚拟机和Android核心库集组成，但由于Dalvik虚拟机采用一种称为JIT（just-in-time）的解释器进行动态编译并执行，导致app运行时较慢；而ART模式则是在用户安装app时进行预编译（ahead-of-time）AOT，减少运行app时动态编译的开销，提升app的运行效率。于此同时ART需要占用更多的存储空间，应用安装和系统的启动时间会延长不少。

（5）Linux内核
Android系统建立在Linux 2.6之上。Linux内核提供了安全性、内存管理、进程管理、网络协议和驱动等核心系统服务。除此之外，Linux内核也是系统硬件和软件叠层之间的抽象层。

参考网址：http://www.360doc.com/content/14/0315/10/7012206_360731070.shtml

----
