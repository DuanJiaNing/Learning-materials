![](http://upload-images.jianshu.io/upload_images/7460499-3bb051bd514ad9a4.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### Android - Bitmap 的加载和 Cache

Android 应用可使用的最大堆内存受到 Android 系统的限制，通常情况为 16 M - 48 M，当应用使用的内存超出限制时就会引发内存溢出错误：`java.lang.OutOfMemoryError`

大多数应用中都会涉及到 Bitmap ，而 Bitmap 往往会占用较多的内存，因此实现高效加载的同时控制好内存占用是加载 Bitmap 时需要遵循的原则。

缓存在很多开发场景中都会涉及，实际开发中经常需要对 Bitmap 进行缓存，通过缓存 Bitmap，能够极大的提高图片加载效率以及用户体验，同时能够减轻内存负担。

#### 1 Bitmap 的加载
 
 Bitmap 指的是一张图片，可以是 png 格式也可以是 jpg 等其他常见的图片格式。
 android 中通常使用 BitmapFactory 来加载 Bitmap，BitmapFactory 提供了如下几种方法从不同位置加载 Bitmap：
 
 - BitmapFactory.decodeByteArray() 从字节数组中加载
 - BitmapFactory.decodeFile() 从文件中加载
 - BitmapFactory.decodeFileDescriptor() 通过”文件描述符“加载
 - BitmapFactory.decodeResource() 从资源中加载，通常为 `drawable`下的资源
 - BitmapFactory.decodeStream() 从输入流中加载

##### 1.1 如何高效加载 Bitmap？
上面 BitmapFactory 的几个加载方法中每种加载方法都会有一个包含 `BitmapFactory.Options` 参数的重载方法，`BitmapFactory.Options`类可以将位图的尺寸按需进行压缩，只加载与 `ImageView`(或其它控件)尺寸相同的压缩尺寸后的位图，这样就可以将加载位图的大小压缩从而在一定程度上避免 OOM。

>使用`BitmapFactory.Options`加载位图时主要用到它的 inSampleSize 参数，即采样率，当 inSampleSize 为 1 时，采样后的图片大小与原始图片的大小相等；当 inSampleSize 大于 1 ，比如 2 时，采样后的图片宽高都为原始图片的 1/2 ，而像素为原始图片的 1/4,。

比如有宽高为 1280 * 720 的一张图片，此时所占内存为 1280 * 720 * 4，当 inSampleSize 为 2 时，其正真加载尺寸为 640 * 360，所占内存为 640 * 360 * 4。

官方文档指出，inSampleSize 的取值应该总是 2 的指数，当不是指数时将向下取整选择最接近的 2 的指数代替。

调整方法如下：
```java
//调整bitmap 大小
    public static Bitmap bitmapResizeFromResource(Resources res, int id, int reqWidth, int reqHeight) {
        final BitmapFactory.Options options = new BitmapFactory.Options();

        //只解析图片的原始宽高，而不正真加载图片
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeResource(res, id, options);

        int inSampleSize = 1;
        if (reqWidth > 0 && reqHeight > 0) {
            final int height = options.outHeight;
            final int width = options.outWidth;

            //如果宽或高有任一者不满足要求就进行调整
            if (height > reqHeight || width > reqHeight) {

                // inSampleSize 为 1 没有作用，使从 2 开始增加
                final int halfHeight = height / 2;
                final int halfWidth = width / 2;

                while ((halfHeight / inSampleSize) >= reqHeight && (halfWidth / inSampleSize) >= reqWidth)
                    inSampleSize *= 2;
            }
        }
        options.inSampleSize = inSampleSize;
        options.inJustDecodeBounds = false;

        return BitmapFactory.decodeResource(res, id, options);
    }
```

#### 2 Android 中的缓存策略

 缓存策略在 Android 开发中有着广泛的使用场景，尤其在图片加载这个场景。

>当程序第一次从网络（或其它来源）加载图片后，就将其缓存到存储设备上，这样下次使用时就不需要再从网络上下载，很多时候为了提高用户体验，往往还会把图片在内存中再缓存一份，这样当应用打算从网络请求一张图片时，程序首先会从内存中去获取，如果内存中没有就去存储设备中获取，如果存储设备中也没有就从网络上下载。

缓存策略并没有统一的标准，一般来说，缓存策略主要包含缓存的添加、获取和删除。缓存的添加和获取都比较好理解，那么为什么要删除呢？这是因为不管是内存缓存还是存储缓存，他们的大小都是有限制的（移动设备的内存可用容量和存储设备，如 SD 卡等的容量也是有限的），当缓存装满时，就需要通过**缓存算法**选择部分缓存数据删除，以添加新的缓存数据。

目前常用的缓存算法是 LRU （Least Recently Used）算法，即最近最少使用算法。这个算法名字是不是很耳熟？《计算机操作系统》在 *存储器管理*  一章中介绍页面置换算法时就有 LRU 算法。

##### 2.1 LruCache
>LruCache 是 Android 提供的一个缓存类，LruCache 时一个泛型类，它内部采用一个 LinkedHashMap 以强引用的方式存储外界的缓存对象，其提供了 get 和 put 方法来完成缓存的获取和添加，当缓存满时，LruCache 会移除较早使用的缓存对象，然后再添加新的缓存对象。

LruCache 支持删除操作，通过 remove 方法可以删除一个指定的缓存对象。
##### 2.2 弱引用、软引用、虚引用、强引用
很早 java API 就添加了弱引用和软引用，引用类在垃圾回收工作的过程中有种要作用。
###### 2.2.1 强引用(Strong Reference)
平常我们开发时创建对象的方式大都是以强引用的方式创建，如`String str = "abc"`中变量 str 就是字符串对象`"abc"` 的一个强引用。
如下代码会移除 str 的强引用：`str = null`，此时垃圾回收器就会在某一时刻回收该对象。
**强引用可以阻止垃圾回收器回收对象。**
###### 2.2.2 弱引用 (WeakReference)
你可以通过如下方式创建一个软引用对象：
`String str = "test"`
`WeakReference<String> wr = new WeakReference<String>(str);`
如果一个对象只有弱引用指向他，当移除强引用时(`str = null`)，垃圾回收器会立即回收该对象。
**弱引用无法阻止垃圾回收器回收对象。**
###### 2.2.3 软引用(SoftReference)
`String str = "test";`
`SoftPreference sr = new SoftPreference(str)`
如果有软引用指向对象，当移除强引用时(`str = null`)，对象不会立即被回收，只有在 JVM 需要内存时，才会回收该对象。
**软引无法阻止垃圾回收器回收对象，但可以延迟回收**
###### 2.2.4 虚引用{PhantomReference}
`String str = "test"`
`PhantomReference pr = new PhantomReference(str)`
当移除强引用时(`str = null`)，拥有虚引用的对象可以在任何时刻被垃圾回收器回收。
**虚引用无法阻止垃圾回收器回收对象。**

参考文章：[Java中弱引用、软引用、虚引用及强引用的区别](https://mp.weixin.qq.com/s?__biz=MjM5NzMyMjAwMA==&mid=2651477541&idx=1&sn=d4a110d011ac32ab0749f9747c1dc126&chksm=bd253a5a8a52b34ca45225ffd917583c46403707acfda0ccd8fbe1fefb1e776b938b6fce6558&mpshare=1&scene=23&srcid=0609Qd4sklt86sXYR0xpyIxE#rd)

##### 2.3 DiskLruCache
>DiskLruCache 用于实现存储设备缓存，即磁盘缓存，它通过将缓存对象写入文件系统从而实现缓存。

DiskLruCache 得到了 android 官方文档的推荐，但它不属于 Android SDK 的一部分，源码可在这里获得：[DiskLruCache.java](http://download.csdn.net/detail/sinyu890807/7709759)

DiskLruCache 的使用不做介绍，可参看这篇文章：[Android DiskLruCache完全解析，硬盘缓存的最佳方案](http://blog.csdn.net/guolin_blog/article/details/28863651)

-----
