### 线程优先级
线程的优先级用数字表示，范围为1~10，默认为5.
每个线程的默认优先级与创建它的父线程相同。数字越大，优先级越高。
Thread类提供了三个常量明确了线程优先级的边界：

- MAX_PRIORITY (10)  //最大优先级
- MIN_PRIORITY (1)   //最小优先级
- NORM_PRIORITY (5) //中等优先级

![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-93a00ffc85b046bd?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
### 线程的终止
Thread.stop方法已经废弃，线程终止线程的根本原则是使run方法运行结束（run方法返回线程就会结束）。Runable的run方法定义中并声明异常，所以终止线程的方法就是run方法返回。

**终止处于“阻塞状态”的线程**
通常，我们通过“中断”方式终止处于“阻塞状态”的线程（即调用线程的interrupt方法）。线程由于调用了sleep(), wait(), join()等方法而进入阻塞状态（这些方法都会抛出InterruptException）；若此时调用线程的interrupt()将线程的中断标记设为true。则线程将产生一个InterruptedException异常（阻塞方法将收到此异常），同时中断标记会被清除（置为false）。
那么只需在收到InterruptException异常时终止run方法执行即可。
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-a1ac21853b0b1ca2?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
**终止处于“运行状态”的线程**
使用“中断标记”，或添加“额外的结束标记”的方式判断线程是否需要结束。
如：调用isInterrupt()判断中断标记是否为true，在需要结束的地方调用线程的interrupt方法。

![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-2d7f00fce5626c9f?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 线程的五种状态
线程从创建运行到结束总是处于下面五个状态之一：**新建状态、就绪状态、运行状态、阻塞状态及死亡状态。**
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-f82c3790c393c95a?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
#### 新建状态(new)
当用new操作符创建一个线程时， 例如new Thread(r)，线程还没有开始运行（未调用其start方法），此时线程处在新建状态。 当一个线程处于新生状态时，程序还没有开始运行线程中的代码。
####就绪状态(Runnable)
一个新创建的线程并不自动开始运行，要执行线程，必须调用线程的start()方法。当线程对象调用start()方法即启动了线程，start()方法创建线程运行的系统资源，并调度线程运行run()方法。当start()方法返回后，线程就处于**就绪状态**。

<u>处于就绪状态的线程并不一定立即运行run()方法，线程还必须同其他线程竞争CPU时间，只有获得CPU时间才可以运行线程。</u> 因为在单CPU的计算机系统中，不可能同时运行多个线程，一个时刻仅有一个线程处于运行状态。因此此时可能有多个线程处于就绪状态。对多个处于就绪状态的线程是由Java运行时系统的线程调度程序(thread scheduler)来调度的。
####运行状态(Running)
当线程获得CPU时间后，它才进入运行状态，真正开始执行run()方法
####阻塞状态(Blocked)
线程运行过程中，可能由于各种原因进入阻塞状态:
1.	线程通过调用sleep方法进入睡眠状态；
2.	线程调用一个在I/O上被阻塞的操作，即该操作在输入输出操作完成之前不会返回到它的调用者；
3.	线程试图得到一个锁，而该锁正被其他线程持有（死锁）。
4.	线程在等待某个触发条件；
...... 

所谓阻塞状态是<u>正在运行的线程没有运行结束，暂时让出CPU</u>，这时其他处于就绪状态的线程就可以获得CPU时间，进入运行状态。
#### 死亡状态(Dead)
有两个原因会导致线程死亡：
1.	run方法正常退出而自然死亡
2.	一个未捕获的异常终止了run方法而使线程猝死。

为了确定线程在当前是否存活着（就是要么是可运行的，要么是被阻塞了），需要使用isAlive方法。如果是可运行或被阻塞，这个方法返回true； 如果线程仍旧是new状态且不是可运行的， 或者线程死亡了，则返回false.

###Object的wait，notify，notifyAll方法
wait()，notify()和notifyAll()方法只能在同步方法或同步代码块里调用。
使用示例：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-c18fc872baa37117?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
输出：
06/12/2017 16:27:13-176 wake up main thread after 3 seconds
06/12/2017 16:27:16-179  i`m wake up
###Thread.sleep和wait区别

####Thread.sleep(mills)
sleep()方法（休眠）是线程类（Thread）的静态方法，调用此方法会**让当前线程暂停执行指定的时间，将执行机会（CPU）让给其他线程，但是对象的锁依然保持**，因此休眠时间结束后会自动恢复（线程回到<u>就绪状态，已获得锁，等待CPU调度</u>）

####Object.wait(mills)
wait()是Object类的方法，调用对象的wait()方法导致**当前线程放弃对象的锁**（线程暂停执行），进入对象的等待池（wait pool），只有调用对象的notify()方法（或notifyAll()方法）时才能唤醒等待池中的线程进入等锁池（lockpool），如果线程重新获得对象的锁就可以进入就绪状态。
###IllegalMonitorStateException
如果在非同步控制方法里调用这些方法（Object的wait、notify、notifyAll），程序能通过编译，但运行的时候，将得到IllegalMonitorStateException异常，并伴随着一些含糊的消息，比如"当前线程不是拥有者"。消息的意思是，**调用wait()，notify()和notifyAll()的线程在调用这些方法前必须"拥有"对象的锁。**当前的线程不是此对象锁的所有者，却调用该对象的notify()，notify()，wait()方法时抛出该异常。
如下几种情况都会得到该异常：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-d55825c3783fbc5e?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
