![](http://upload-images.jianshu.io/upload_images/7460499-830bbb655cdb2cdd.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
### Android-Android 的消息机制
>Android 的消息机制主要指的是 Handler 的运行机制，Handler 是 Android 消息机制的上层接口，通过 Handler 可以轻松的将一个任务切换到 Handler 所在的线程中去执行，由于 Android 开发规范的限制，我们不能在非 UI 线程中更新 UI，同时不应该也不能在 UI 线程中进行耗时的 I/O 操作或者进行网络访问，这时就需要使用 Handler。

#### 1 Handler、MessageQueue、Looper 概述
>Handler 的运行需要底层的 MessageQueue 和 Looper 的支撑。

- MessageQueue：其中文翻译是“消息队列”，内部存储了一组消息（Message），虽然叫消息队列，但其内部使用的数据结构并不是队列，而是单链表。
- Looper：Looper 会以无限循环的方式去 MessageQueue 中查找是否还有未处理的消息或新消息，有的话就处理，没有则等待。
- ThreadLocal：Looper 中使用到了 ThreadLocal ，ThreadLocal 并不是线程，我们知道 Handler 创建时会采用当前线程的 Looper 来构造消息循环系统，那他怎么找到当前线程的 Looper 实例呢？ThreadLocal 可以在每个线程中存储同一个对象的不同副本，即不同线程可以调用同一个 ThreadLocal 实例的方法获取属于自己的 Looper 实例，

#### 2 Android 消息机制概述
##### 2.1 ViewRootImpl 
>ViewRootImpl 是链接 WindowManager 和DecorView 的纽带，另外 View 的绘制也是通过ViewRootImpl 来完成的。

它的主要作用总结如下：

- 链接WindowManager和DecorView的纽带，更广一点可以说是Window和View之间的纽带。
- 完成View的绘制过程，包括measure、layout、draw过程。
- 向DecorView分发收到的用户发起的event事件，如按键，触屏等事件。

ViewRootImpl 是View 树的树根，但它不是View，它实现了 View 与 WindowManager 之间的通信协议，
参考链接：[Android中的ViewRootImpl类源码解析](http://www.2cto.com/kf/201606/519988.html)
##### 2.2 系统怎么知道你在哪里更新的 UI
Android 规定开发着不能在非 UI 线程中更新 UI，如果你不遵守这个规定，那么将引发运行时异常`CalledFromWrongThreadException`，其原因在于 ViewRootImpl 对 UI 操作做了验证， ViewRootImpl 作为视图层次结构的顶部，对 UI 的访问操作大部分都会传递到 ViewRootImpl  中，ViewRootImpl 会在 checkThread 方法中检查访问操作是否在 UI 线程，不是的话就会抛出异常。
<h5><center>ViewRootImpl # checkThread() </center></h6>
```java
   void checkThread() {
        if (mThread != Thread.currentThread()) {
            throw new CalledFromWrongThreadException(
                    "Only the original thread that created a view hierarchy can touch its views.");
        }
    }
```
##### 2.3 系统为什么不允许在 UI 线程中访问 UI 呢？
>这是因为 Android 的 UI 控件不是线程安全的，如果在多线程中并发访问可能会导致 UI 控件处于不可预期的状态，那为什么不对 UI 控件的访问加上锁呢？缺点有两个：首先加上锁机制会让 UI 访问的逻辑变得复杂；其次锁机制会降低 UI 访问的效率，因为锁机制会阻塞某些线程的执行。


#### 3 ThreadLocal 的工作原理
><u><b>java.lang.ThreadLocal</b></u>
>
>该类提供了线程局部 (thread-local) 变量。这些变量不同于它们的普通对应物，因为访问某个变量（通过其 get 或 set 方法）的每个线程都有自己的局部变量，它独立于变量的初始化副本。ThreadLocal 实例通常是类中的 private static 字段，它们希望将状态与某一个线程（例如，用户 ID 或事务 ID）相关联。 
>
>ThreadLocal 是一个线程内部的数据存储类，通过它可以在指定的线程中存储数据，数据存储后，只有在指定线程中可以获取到数据，对于其他线程来说则无法获取到数据。

举个栗子：
```java
public class Main {

    ThreadLocal<Integer> mLocal = new ThreadLocal<>();

    public static void main(String[] args) {

        Main test = new Main ();
		test.mLocal.set(30);
        test.print("thread: " + Thread.currentThread().getName() + " " + test.mLocal.get());
        test.test();

    }

    private void print(String string) {
        System.out.println(string);
    }

    void test() {
        
        new Thread("Thread#1") {
            @Override
            public void run() {
                mLocal.set(31);
                print("thread: " + this.getName() + " " + mLocal.get());
            }
        }.start();


        new Thread("Thread#2") {
            @Override
            public void run() {
                mLocal.set(32);
                print("thread: " + this.getName() + " " + mLocal.get());
            }
        }.start();


        new Thread("Thread#3") {
            @Override
            public void run() {
                mLocal.set(32);
                print("thread: " + this.getName() + " " + mLocal.get());
            }
        }.start();
    }
}

```
上面的例子在执行 test 方法时，启动了三个线程，在这三个线程中分别修改了 mLocal 的值，当各自线程通过调用 ThreadLocal 的 get 方法取值时取到的值是不同的，即各个线程有属于自己的一个值。
 
```dos
F:\javaStuff>javac Main.java
F:\javaStuff>java Main
thread: main 30
thread: Thread#1 31
thread: Thread#2 32
thread: Thread#3 32
```
>ThreadLocal 之所以有这么奇妙的功能，是因为不同线程访问同一个 ThreadLocal 对象的 get 方法，ThreadLocal 内部会从各自的线程中取出一个数据实体，然后再从数据实体中取得对应的 value 值。

那这个线程私有的数据实体在哪呢？
先看看 ThreadLocal 的 get 方法
<h5><center>JDK 1.8.0_45  ThreadLocal # get () </center></h6>
```java
public T get() {
        Thread t = Thread.currentThread();
        ThreadLocalMap map = getMap(t);
        if (map != null) {
            ThreadLocalMap.Entry e = map.getEntry(this);
            if (e != null)
                return (T)e.value;
        }
        return setInitialValue();
    }
```
可以看到这里使用了 ThreadLocalMap.Entry 来保存数据，在《Android 开发艺术探索》一书中说的是使用 ThreadLocal.Values 来保存数据，这里的不同应该是由于 JDK 版本升级过程导致的。
再看看在 Thread 中 对 ThreadLocalMap 的引用：
<h5><center>Thread # threadLocals </center></h6>
```java
/* ThreadLocal values pertaining to this thread. This map is maintained
     * by the ThreadLocal class. */
    ThreadLocal.ThreadLocalMap threadLocals = null;
```
从 ThreadLocal 的 get方法可以知道` getMap(t)`操作会得到当前线程的 threadLocals 对象，看看 getMap 方法。
<h5><center>ThreadLocal # getMap(Thread t)</center></h6>
```java
ThreadLocalMap getMap(Thread t) {
        return t.threadLocals;
    }
```
到这里就很清晰了，getMap 方法会返回当前线程的 threadLocals  。
##### 3.1 ThreadLocal 使用场景
- 当某些数据是以线程作为作用域并且不同线程具有不同的数据副本。
- 复杂逻辑下的数据传递，比如监听器的传递，有些时候一个线程中的任务过于复杂，这可能表现为函数调用栈比较深以及代码入口的多样性，在这种情况下我们又需要监听器贯穿整个线程的执行过程，这个时候该怎么办？这个时候就可以采用 ThreadLocal ，采用 ThreadLocal 可以让监听器作为线程内的全局对象，在线程内部只需调用 ThreadLocal 的 get 方法就能获得监听器。

#### 4 MessageQueue 的工作原理
一个 Handler 可以正常工作的线程只会有一个 MessageQueue 的实例。
MessageQueue 主要包含两个操作：

- 插入：enqueueMessage
- 删除（读取）：next

##### 4.1 enqueueMessage 方法
<h5><center>MessageQueue # enqueueMessage(Message msg, long when) </center></h6>
```java
    boolean enqueueMessage(Message msg, long when) {
        ...
        synchronized (this) {
            ...
            msg.markInUse();
            msg.when = when;
            Message p = mMessages;
            boolean needWake;
            if (p == null || when == 0 || when < p.when) {
                // New head, wake up the event queue if blocked.
                msg.next = p;
                mMessages = msg;
                needWake = mBlocked;
            } else {
                ...
                Message prev;
                for (;;) {
                    prev = p;
                    p = p.next;
                    if (p == null || when < p.when) {
                        break;
                    }
                    if (needWake && p.isAsynchronous()) {
                        needWake = false;
                    }
                }
                msg.next = p; // invariant: p == prev.next
                prev.next = msg;
            }
            ...
        }
        return true;
    }
```
`  if (p == null || when == 0 || when < p.when) {..`： 其中 p 为下一个待处理的消息，如果 p 为 null，或插入消息的执行时间为“立刻”(when  == 0)，或插入消息执行时间比下一个待处理消息早，那么插入消息就做为新的消息队列头，将其插入对头（` msg.next = p; mMessages = msg;`）。
若该判断不满足，即当前消息队列不为空，插入消息的执行时间不是“立刻”，则将其插入队列（按执行时间排序）。

##### 4.1 next 方法

<h5><center>MessageQueue # next() </center></h6>
```java
Message next() {
        ...
        for (;;) {
            if (nextPollTimeoutMillis != 0) {
                Binder.flushPendingCommands();
            }

            nativePollOnce(ptr, nextPollTimeoutMillis);

            synchronized (this) {
                // Try to retrieve the next message.  Return if found.
                final long now = SystemClock.uptimeMillis();
                Message prevMsg = null;
                Message msg = mMessages;
                if (msg != null && msg.target == null) {
                    // Stalled by a barrier.  Find the next asynchronous message in the queue.
                    do {
                        prevMsg = msg;
                        msg = msg.next;
                    } while (msg != null && !msg.isAsynchronous());
                }
                if (msg != null) {
                    if (now < msg.when) {
                        // Next message is not ready.  Set a timeout to wake up when it is ready.
                        nextPollTimeoutMillis = (int) Math.min(msg.when - now, Integer.MAX_VALUE);
                    } else {
                        // Got a message.
                        mBlocked = false;
                        if (prevMsg != null) {
                            prevMsg.next = msg.next;
                        } else {
                            mMessages = msg.next;
                        }
                        msg.next = null;
                        if (DEBUG) Log.v(TAG, "Returning message: " + msg);
                        msg.markInUse();
                        return msg;
                    }
                } else {
                    // No more messages.
                    nextPollTimeoutMillis = -1;
                }
                ...
            }
		...
        }
    }
```
>可以发现 next 方法是一个无限循环方法，如果消息队列中没有消息，那么 next 方法会一直阻塞。当有新的消息到来时，next 方法会返回这条消息并将其从单链表中移除。

MessageQueue 的 next 方法会被 Looper 的 loop 方法调用，从而使 loop 方法也成为阻塞方法。

#### 4 Looper 的工作原理
一个 Handler 可以正常工作的线程只会有一个 Looper 的实例。
>Looper 在 Android 的消息机制中扮演者消息循环的角色，具体来说就是它会不断的从 MessageQueue 中查看是否有新消息，如果有新消息就立刻处理，否则就一直阻塞在那里。

先看看 Looper 的构造方法：
```java
	private Looper(boolean quitAllowed) {
        mQueue = new MessageQueue(quitAllowed);
        mThread = Thread.currentThread();
    }
```
可以看到 Looper 的构造方法是私有的，即外界无法通过 new 关键字创建其实例。
构造方法会实例化 MessageQueue 的实例 ` mQueue `。

##### 4.1 在非 UI 线程使用 Handler
- 如果想在一个子线程（非 UI 线程）中正常的使用 Handler ，就必须让当前线程拥有一个 Looper（Looper.prepare()），并且执行其 loop （Looper.loop()）方法。
```java
 private Handler mHandler;
    private void test() {
        new Thread() {
            @Override
            public void run() {
                mHandler = new Handler();
                Looper.prepare();
                Looper.loop();
            }
        }.start();
    }
```
- 看看 Looper.prepare() 方法
<h5><center>Looper # prepare() </center></h6>
```java
 public static void prepare() {
        prepare(true);
    }

  private static void prepare(boolean quitAllowed) {
       if (sThreadLocal.get() != null) {
           throw new RuntimeException("Only one Looper may be created per thread");
       }
       sThreadLocal.set(new Looper(quitAllowed));
   }
```
` sThreadLocal.get()` 返回结果不为空表示当前线程的 Looper.prepare() 方法已经被调用过，即当前线程已存在 Looper 实例。这就可以保证一个线程只有一个Looper，同时也保证了一个线程只有一个 MessageQueue （参照 Looper 构造方法可知）。
这里有个关键的变量 ` sThreadLocal`，看看它的声明：
<h5><center>Looper # sThreadLocal  </center></h6>
```java
// sThreadLocal.get() will return null unless you've called prepare().
    static final ThreadLocal<Looper> sThreadLocal = new ThreadLocal<Looper>();
```
参照上面对 ThreadLocal 的说明就可以知道 ` sThreadLocal.get()`返回的是当前线程对应的那个 Looper 对象。 

- Looper.loop() 方法

>Looper 最重要的一个方法是 loop 方法，只有调用了 loop 后，消息循环系统才会正真的起作用。

 <h5><center>Looper # loop() </center></h6>
```java
 public static void loop() {
        final Looper me = myLooper();
        if (me == null) {
            throw new RuntimeException("No Looper; Looper.prepare() wasn't called on this thread.");
        }
        final MessageQueue queue = me.mQueue;

        // Make sure the identity of this thread is that of the local process,
        // and keep track of what that identity token actually is.
        Binder.clearCallingIdentity();
        final long ident = Binder.clearCallingIdentity();

        for (;;) {
            Message msg = queue.next(); // might block
            if (msg == null) {
                // No message indicates that the message queue is quitting.
                return;
            }

            // This must be in a local variable, in case a UI event sets the logger
            final Printer logging = me.mLogging;
            if (logging != null) {
                logging.println(">>>>> Dispatching to " + msg.target + " " +
                        msg.callback + ": " + msg.what);
            }

            final long traceTag = me.mTraceTag;
            if (traceTag != 0 && Trace.isTagEnabled(traceTag)) {
                Trace.traceBegin(traceTag, msg.target.getTraceName(msg));
            }
            try {
                msg.target.dispatchMessage(msg);
            } finally {
                if (traceTag != 0) {
                    Trace.traceEnd(traceTag);
                }
            }

            if (logging != null) {
                logging.println("<<<<< Finished to " + msg.target + " " + msg.callback);
            }

            // Make sure that during the course of dispatching the
            // identity of the thread wasn't corrupted.
            final long newIdent = Binder.clearCallingIdentity();
            if (ident != newIdent) {
                Log.wtf(TAG, "Thread identity changed from 0x"
                        + Long.toHexString(ident) + " to 0x"
                        + Long.toHexString(newIdent) + " while dispatching to "
                        + msg.target.getClass().getName() + " "
                        + msg.callback + " what=" + msg.what);
            }

            msg.recycleUnchecked();
        }
    }
```
- ` myLooper()`：方法可以获得当前线程的 Looper，该方法是 public 的，在类外也可以调用。
- ` Message msg = queue.next(); // might block`：next 方法在上面分析*MessageQueue 的工作原理*时已经分析过了，会next 方法是一个无限循环方法，如果消息队列中没有消息，那么 next 方法会一直阻塞。当有新的消息到来时，next 方法会返回这条消息并将其从单链表中移除。如果 next 方法返回 null，那么 loop 循环就会结束，
- ` msg.target.dispatchMessage(msg);`：当 next 有消息返回时，Looper 就会处理这条消息，这里的 msg.target 时发送这条消息的 Handler 对象，这样 Handler 发送的消息最终又交给它的 dispatchMessage 方法来处理了。这里要注意的是，dispatchMessage 方法会是在创建 Handler 的线程中执行的，这样就成功的将代码逻辑切换到指定的线程中去执行。（dispatchMessage  方法将在 <u>Handler 的工作原理</u> 中分析）

##### 4.2 主线程（ActivityThread ）的消息循环
Android 的主线程由 ActivityThread 类表示。
>Looper 除了 prepare 方法外，还提供了 prepareMainLooper 方法，这个方法主要是给主线程也就是 ActivityThread 创建 Looper 使用的，其本质也是通过 prepare 方法来实现的。
>
>Java 程序少不了会有一个执行入口 main 方法，那 Android 程序的 main方法在哪呢？
>其实 Android 的 main 方法被包装在 ActivityThread 类中。所有的 Android 程序都有且仅有一个ActivityThread 类的实例，ActivityThread 所在的线程即为主线程（UI 线程）。

Android 程序 从ActivityThread 的 main 方法开始执行，调用 prepareMain 方法为主线程创建一个 Looper 和 一个 MessageQueue，然后创建一个 ActivityThread 对象，在 ActivityThread 的初始化代码中会创建一个 Handler 对象。接着 main 方法会调用 Looper.loop() 方法进入消息循环，不断地从消息队列中读取并处理消息。

参考链接：
[ Android中线程那些事](http://blog.csdn.net/lfdfhl/article/details/51279160)
[ActivityThread的main方法究竟做了什么？](http://www.jianshu.com/p/0efc71f349c8)
<h5><center>ActivityThread  # main(String[] args)</center></h6>
```java
   public static void main(String[] args) {
        ...
        Looper.prepareMainLooper();

        ActivityThread thread = new ActivityThread();
        thread.attach(false);

        if (sMainThreadHandler == null) {
            sMainThreadHandler = thread.getHandler();
        }

        if (false) {
            Looper.myLooper().setMessageLogging(new
                    LogPrinter(Log.DEBUG, "ActivityThread"));
        }

        // End of event ActivityThreadMain.
        Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
        Looper.loop();

        throw new RuntimeException("Main thread loop unexpectedly exited");
    }
```
这里就产生了一个挺有趣的问题：<u><b>ActivityThread 的 main 方法会在 loop 方法处不断循环，没有要处理的消息就会 *阻塞*，那为什么这里的阻塞不会引发 ANR（Application Not Responding） 呢？</b></u>

想象这样一种情况，如果不执行 loop 方法，那么 Android 程序的主线程一运行完程序就会退出！即用户才打开 APP ，APP 就自己关了，这显然是不可以的。
那 Android 是怎么实现阻塞而不引发 ANR 呢？

这里需要先了解 ANR 的产生原因：

-	当前的事件没有机会得到处理（即主线程正在处理前一个事件，前一个事件没有及时的完成或者 looper 被某种原因阻塞住了）
- 当前的事件正在处理，但没能在规定时间内完成（广播事件处理的 10s 限定，输入事件分发 5s ，前台服务 20s 等）
 	
由 ANR 产生的原因可以知道一个关键的因素是 —— **没有及时完成**，即在规定时间内没有完成，而主线程 loop 循环这个操作系统并没有对其有时间限定，而 loop 循环内部在处理消息时，对某个具体消息的执行有时是有时间限定的，超过了这个时间就会引发 ANR。

##### 4.3 Looper 的退出方式
>Looper 也是可以退出的，Looper 提供了 quit 和 quitSafely 来退出一个 Looper

- quit 和 quitSafely 方法
这两个方法的区别在于： quit 方法会直接退出 Looper，而 quitSafely 只是设定一个退出标记，然后把消息队列中的已有消息处理完成后才安全退出。
<h5><center>Looper # quit() & quitSafely()</center></h6>
```java
 public void quit() {
        mQueue.quit(false);
    }

public void quitSafely() {
        mQueue.quit(true);
    }
```
可以看到都调用了 MessageQueue 的 quit 方法。看看 MessageQueue 的 quit 方法
<h5><center>MessageQueue # quit(boolean safe)</center></h6>
```java
 void quit(boolean safe) {
        if (!mQuitAllowed) {
            throw new IllegalStateException("Main thread not allowed to quit.");
        }

        synchronized (this) {
            if (mQuitting) {
                return;
            }
            mQuitting = true;

            if (safe) {
                removeAllFutureMessagesLocked();
            } else {
                removeAllMessagesLocked();
            }

            // We can assume mPtr != 0 because mQuitting was previously false.
            nativeWake(mPtr);
        }
    }
```
若当前的 MessageQueue 实例属于主线程，那么调用主线程 Looper 的 quit（或 quitSafely）都会抛出这个异常，` mQuitAllowed`变量在 Looper 的 prepareMainLooper 方法中会被赋值为 false（具体在` private static void prepare(boolean quitAllowed)`方法中赋值）。

参照 loop 方法可知当 MessageQueue 的 next 方法返回为 null 时，loop 循环就会退出，由此可知，` removeAllMessagesLocked`方法会直接将下一个待处理消息置为 null，这样 next 方法调用时就会返回 null；` removeAllFutureMessagesLocked`方法则会在当前消息队列的队尾添加一个 null 消息，并拒绝再接收消息，那么当当前已有的消息处理完就会返回给 next 方法 null，loop 循环就会结束。


#### 5 Handler 的工作原理
>Handler 的工作主要包括消息的发送和接收。消息的发送可以通过 post 的一系列方法以及 send 的一系列方法来实现，post 的一系列方法最终是通过 send 的一系列方法来实现的。

send 一系列方法：

- 立即发送消息一条消息：sendMessage
- 发送空的延迟消息：sendEmptyMessageDelayed
- 立即发送一条空消息：sendEmptyMessage         
- 发送一条消息到队列头：sendMessageAtFrontOfQueue 
- 在指定时间发送消息：sendMessageAtTime         
- 发送延迟消息：sendMessageDelayed        
- 在指定时间发送空消息：sendEmptyMessageAtTime   

post 一系列方法：发送 Runnable 对象，内部调用的是 send 的一系列方法

- post 
- postDelayed  
- postAtFrontOfQueue   
- postAtTime
- sendMessageAtTime 

send 和 post 的一系列方法最终的函数调用（真正进行消息发送操作）为 ` sendMessageAtTime`，或是`sendMessageAtFrontOfQueue`方法，其他的 sendXXX 和 postXXX 方法最后都会转到这两个方法，对这两个方法进行分析如下

##### 5.1 sendMessageAtTime 方法
send 的一系列方法中 sendMessage、sendEmptyMessageDelayed、sendEmptyMessage、sendMessageDelayed、sendEmptyMessageAtTime 方法；post 的一系列方法 post、postDelayed、postAtTime 方法的最终调用都为 sendMessageAtTime 方法。
<h5><center>Handler # sendMessageAtTime(Message msg, long uptimeMillis) </center></h6>
```java
public boolean sendMessageAtTime(Message msg, long uptimeMillis) {
        MessageQueue queue = mQueue;
        if (queue == null) {
            RuntimeException e = new RuntimeException(
                    this + " sendMessageAtTime() called with no mQueue");
            Log.w("Looper", e.getMessage(), e);
            return false;
        }
        return enqueueMessage(queue, msg, uptimeMillis);
    }
```
方法的第二个参数` uptimeMillis`在文档中的解释为：传递该消息的绝对时间，该时间应该将 SystemClock.uptimeMillis（开机的到现在的毫秒数，不包括系统睡眠时间）的值作为基数，在其基础上加上延迟时间。

` sendMessageDelayed`或`sendMessageDelayed`方法内部会调用`sendMessageAtTime`方法，调用形式是这样的：` sendMessageAtTime(msg, SystemClock.uptimeMillis() + delayMillis)`,延迟时间（delayMillis）加上开机到现在的毫秒数即为发送该条消息的绝对时间，`sendMessage`方法中会调用`sendMessageDelayed`方法，此时传入的`delayMillis`就为 0 。

可以看到该方法内部首先会检查 mQueue 是否为 null，为 null 就表示当前线程没有 MessageQueue 对象，为什么没有 MessageQueue 对象，回想一下，MessageQueue 对象应该在哪里实例化? 在Looper 的构造函数里，那 Looper 的构造函数又在哪里调用呢？ 在 Looper.prepare() .......，对了，很多时候程序抛出这个异常就是因为你忘了调用 Looper.prepare() 方法。 

接下来调用 enqueueMessage 方法，看名字就可以猜出来其内部肯定会调用 MessageQueue 的 enqueueMessage 方法将消息插入到消息队列中，而事实也是这样的。
<h5><center>Handler # enqueueMessage(MessageQueue queue, Message msg, long uptimeMillis) </center></h6>
```java
private boolean enqueueMessage(MessageQueue queue, Message msg, long uptimeMillis) {
        msg.target = this;
        if (mAsynchronous) {
            msg.setAsynchronous(true);
        }
        return queue.enqueueMessage(msg, uptimeMillis);
    }
```

##### 5.2 sendMessageAtFrontOfQueue 方法
send 一系列方法的 sendMessageAtFrontOfQueue；post 一系列方法的 postAtFrontOfQueue 方法的最终调用为  sendMessageAtFrontOfQueue 方法。
<h5><center>Handler # sendMessageAtFrontOfQueue(Message msg) </center></h6>
```java
    public final boolean sendMessageAtFrontOfQueue(Message msg) {
        MessageQueue queue = mQueue;
        if (queue == null) {
            RuntimeException e = new RuntimeException(
                this + " sendMessageAtTime() called with no mQueue");
            Log.w("Looper", e.getMessage(), e);
            return false;
        }
        return enqueueMessage(queue, msg, 0);
    }
```
该方法同样会检查 mQueue 是否为 null。
注意这里调用` enqueueMessage`方法时传入的 `uptimeMillis` 的值为 0 ，你可以往上翻翻，找到 MessageQueue 的 `enqueueMessage(Message msg, long when)`方法的分析部分，此时传到 MessageQueue 的 enqueueMessage 方法里的 	`when`的值就为 0 ，通过`MessageQueue#enqueueMessage`方法就可以知道，当 `when == 0`，该条消息将被插入消息队列的队头位置。

##### 5.3 总结
>可以发现 Handler 发送消息的过程仅仅是向消息队列中插入一条消息，MessageQueue 的 next 方法就会返回 这条消息给 Looper，Looper 收到消息后开始处理，最终消息由 Handler 处理，即 Handler 的 dispatchMessage 方法会被调用，这时 Handler 就进入处理消息的阶段。 

##### 5.4 Handler 的 dispatchMessage 方法
>dispatchMessage 方法会检查消息（Message）的 callback 是否为 null，不为 null 就 通过 handleCallback 来处理消息，实际上就是 Handler 的一系列 post 方法传过来的 Runable 参数。

 <h5><center>Handler # dispatchMessage(Message msg) </center></h6>
```java
    public void dispatchMessage(Message msg) {
        if (msg.callback != null) {
            handleCallback(msg);
        } else {
            if (mCallback != null) {
                if (mCallback.handleMessage(msg)) {
                    return;
                }
            }
            handleMessage(msg);
        }
    }
```
dispatchMessage 方法先会检查消息的 callback 是否为 null，其次检查 Handler 的 mCallback 是否为 null，不为 null 则调用其 handleMessage 方法，若该方法返回 true 则消息处理完成，否则（最后）调用 Handler 对象的 handleMessage 方法处理消息。

这里有个小技巧——Handler 的 mCallback 域，通常我们使用 Handler 的方法是使用其导出来（子类），或者是匿名内部类（实质也是子类），这时我们就需要覆写 Handler 的 handleMessage 方法。那如果不想通过继承的方式使用 Handler 呢，mCallback 就是一种途径：
Handler 提供了一个构造函数` public Handler(Callback callback)`，这就允许我们通过传参的方式使用 Handler。

上面提到的几个变量，Message 的 callback，Handler 的 mCallback，callback 是 Runnable 对象，mCallback 为 Handler 的内部类。

 <h5><center>Handler # Callback  </center></h6>
```java
public interface Callback {
        public boolean handleMessage(Message msg);
    }
```

----------

文章部分内容摘抄自《Android 开发艺术探索》第 10 章 —— Android 的消息机制，加上自己的理解和总结，可能有错误，欢迎指正。

----
