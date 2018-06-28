## ThreadLocal
参考文章：[聊一聊 Spring 中的线程安全性](http://mp.weixin.qq.com/s?__biz=MjM5NzMyMjAwMA==&mid=2651479529&idx=2&sn=956b7870b4b4ddda694ddb9dd45e4115&chksm=bd2531968a52b880c62882188504e2f69fbc93765a6359988de2861774a6fe6df9b0ee941ec9&mpshare=1&scene=23&srcid=1201lPTSxvajWkRcQCP5vKmp#rd )

### 1. 何为 ThreadLocal？
ThreadLocal是一个为线程提供线程局部变量的工具类。它的思想也十分简单，就是**为线程提供一个线程私有的变量副本，这样多个线程都可以随意更改自己线程局部的变量，不会影响到其他线程**。不过需要注意的是，ThreadLocal提供的只是一个浅拷贝，如果变量是一个引用类型，那么就要考虑它内部的状态是否会被改变，想要解决这个问题可以通过重写ThreadLocal的initialValue()函数来自己实现深拷贝。

ThreadLocal与像synchronized这样的锁机制是不同的。首先，它们的应用场景与实现思路就不一样，锁更强调的是如何**同步多个线程去正确地共享一个变量**，ThreadLocal则是为了解决**同一个变量如何不被多个线程共享**。从性能开销的角度上来讲，如果锁机制是用时间换空间的话，那么ThreadLocal就是用空间换时间。

ThreadLocal中含有一个叫做ThreadLocalMap的内部类，该类为一个采用线性探测法实现的HashMap。它的key为ThreadLocal对象而且还使用了WeakReference，ThreadLocalMap正是用来存储变量副本的。
### 2. 怎么使用？
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-85fe67c86362f598?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
输出：
t2 null
t1 User(name=rose, age=22, birthday=Fri Dec 01 11:22:40 CST 2017) true
t3 User(name=jack, age=23, birthday=Mon Dec 04 11:22:40 CST 2017) true
 
### 3. 原理
ThreadLocal通过get方法获得线程私有的变量，那get方法是怎么获取的呢？
<p>要获得当前线程私有的变量副本需要调用get()函数。首先，它会调用getMap()函数去获得当前线程的ThreadLocalMap，这个函数需要接收当前线程的实例作为参数。如果得到的ThreadLocalMap为null，那么就去调用setInitialValue()函数来进行初始化，如果不为null，就通过map来获得变量副本并返回。</p>
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-b552ee4b2704ebc4?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
 <br>
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-067e0004a07420d8?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
 <br>
setInitialValue()函数会去先调用initialValue()函数来生成初始值，该函数默认返回null，**我们可以通过重写这个函数来返回我们想要在ThreadLocal中维护的变量**。之后调用getMap方法获得ThreadLocalMap，如果该map已经存在，那么就用新获得value去覆盖旧值，否则就调用createMap()函数来创建新的map。
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-154d4922c56751f3?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
### 4. 覆写initialValue方法
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-dcfc52faa2bd8664?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

仍然使用上面的例子，输出为：
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-388a6fbc3eedacd1?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
### 5. set和remove方法
ThreadLocal的set()与remove()方法要比get()的实现要简单，都只是通过getMap()来获得ThreadLocalMap然后对其进行操作。
[图片上传失败...(image-7fdb3-1517590419123)]
[图片上传失败...(image-c68151-1517590419123)]

### 6. ThreadLocalMap
<p>不难想到，有一种普遍的方法是通过一个全局的线程安全的Map来存储各个线程的变量副本，但是这种做法已经完全违背了ThreadLocal的本意，设计ThreadLocal的初衷就是<b>为了避免多个线程去并发访问同一个对象</b>，尽管它是线程安全的。而在每个Thread中存放与它关联的ThreadLocalMap是完全符合ThreadLocal的思想的。</p>
<p>当想要对线程局部变量进行操作时，只需要把Thread作为key来获得Thread中的ThreadLocalMap即可。这种设计相比采用一个全局Map的方法会多占用很多内存空间，但也因此不需要额外的采取锁等线程同步方法而节省了时间上的消耗。（空间换时间）</p>
### 7. ThreadLocal中的内存泄漏及解决
如果ThreadLocal被设置为null后，而且没有任何强引用指向它，根据垃圾回收的可达性分析算法，ThreadLocal将会被回收。这样一来，ThreadLocalMap中就会含有key为null的Entry，而且ThreadLocalMap是在Thread中的，只要线程迟迟不结束，这些无法访问到的value会形成内存泄漏。为了解决这个问题，<u>ThreadLocalMap中的getEntry()、set()和remove()函数都会清理key为null的Entry。</u>
 ![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-438f0974644b5646?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
 
<b>强引用key</b>：ThreadLocal被设置为null，由于ThreadLocalMap持有ThreadLocal的强引用（有一个强引用），如果不手动删除，那么ThreadLocal将不会回收，产生内存泄漏。

<b>弱引用key</b>：ThreadLocal被设置为null，由于ThreadLocalMap持有ThreadLocal（只剩弱引用）的弱引用，即便不手动删除，ThreadLocal仍会被回收，ThreadLocalMap在之后调用set()、getEntry()和remove()函数时会清除所有key为null的Entry。

但要注意的是，ThreadLocalMap仅仅含有这些被动措施来补救内存泄漏问题。如果你在之后没有调用ThreadLocalMap的set()、getEntry()和remove()函数的话，那么仍然会存在内存泄漏问题。

在使用线程池的情况下，如果不及时进行清理，内存泄漏问题事小，甚至还会产生程序逻辑上的问题。所以，为了安全地使用ThreadLocal，必须要像每次使用完锁就解锁一样，在每次使用完ThreadLocal后都要调用remove()来清理无用的Entry。
