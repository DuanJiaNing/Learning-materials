## NIO
Jdk 1.4 引入，nio的目标是快速移动大量数据。

Nio速度的提高来自于使用的结构更接近与操作系统执行io的方式：通道和缓冲器。
我们并没有直接和通道交互，而是和缓冲器（ByteBuffer）。

旧io中的FileInputStream、FileOutputStream和RandomAccessFile（都为字节操纵流）支持获得通道（getChannel）。而字符流则不能获得通道。

ByteBuffer提供了一系列的putXXX方法对缓冲区进行填充。亦可通过warp工厂方法的方式、allocate方法指定容量的方式获得实例。

## 基本使用
通过ByteBuffer与通道打交道。
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-1ba7eb2af678c8b4?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
在当缓冲器中的数据即将被读取之前，需要调用缓冲器的flip方法，让缓冲器做好被读取的准备，这样能获得更快的速度；当缓冲器需要重新填充数据时，调用缓冲器的clear方法清除缓存数据。
 
上述方法完成了tip.txt文件的复制。也可以通过下面的transferTo方法实现：
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-8e25147f84080321?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
### 将字节编码为字符
如果直接将字节读取并转换将无法正确解码，同样的直接使用ByteBuffer的asCharBuffer并调用CharBuffer的toString方法也将产生编码错误，因为不知道字符是如何编码的，也就无法正确解码。

使用平台默认编码解码：
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-37b7df1bebbf3633?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

以字符的形式写入之后再读取：
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-0079412af1597646?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

存入时指定字符集：
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-fcd72aab71266c10?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
 
## ByteBuffer
ByteBuffer是将数据移进移出通道的唯一方式，并且我们只能创建一个独立的基本类型缓冲器（ByteBuffer，DoubleBuffer，ShortBuffer等），或者使用ByteBuffer的asXXXBuffer方法从ByteBuffer获得。

ByteBuffer由数据和可以高效操作这些数据的四个索引组成，这四个索引为：

- mark：标记
- position：位置
- limit：界限
- capacity：容量

### 操纵索引的方法
capacity()：获得缓冲区容量
clear()：情况缓冲区，将position置为0，limit置为容量。可用于覆写缓冲区
flip()：将limit置为position，position置为0.用于准备从缓冲区读取已经写入的数据
limit()：获得limit值
limit(int lim)：设置limit值
mark()：将mark置为position
position()：获得position
position(int pos)：设置position
remaining()：获得 limit – position
hasremaining()：若有介于position和limit之间的元素，则返回true
reset()：将position设置为上一次mark的地方，如果之前没mark会出错，如果mark之后重新设置过position，而且新的position大于mark，mark失效。（源码）
get()：获得当前position对应的值，之后position自增1
get(int index)：获得指定值，不自增
put(*)：向缓冲区中添加元素，同时每添加一个元素，position会自增1
rewind()：执行position=1，mark=-1
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-f830a9ab3c11ee6e?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## MappedByteBuffer –内存映射文件
内存映射文件允许我们创建和修改那些太大（如2G）而不能放入内存的文件。

继承于ByteBuffer，是一种特殊类型的缓冲器，创建时必须指定映射文件的起始位置和映射区域的长度，即我们可以映射某个大文件中的较小部分。
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-f580ce2676ffb620?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

此外，nio映射文件访问比起io在性能上有显著提高，表现出更快的读写速度。

文章大部分内容摘抄自《Thinking in java》第18章：18.10 新I/O
