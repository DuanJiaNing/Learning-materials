java - ReentrantLock和Condition实现生产者-消费者

## ReentrantLock
重入锁（ReentrantLock）是一种递归无阻塞的同步机制。

一个可重入的互斥锁（Lock），它具有与使用 synchronized 方法和语句所访问的隐式监视器锁定相同的一些基本行为和语义，但功能更强大。
1.	ReentrantLock 将由当前已经成功获得锁，并且还没有释放锁的线程所拥有。
2.	使用lock获得锁，unlock释放锁。
3.	可以使用 isHeldByCurrentThread() 和 getHoldCount() 方法来检查当前线程是否获得锁及调用了几次

### lock
1.	当锁没有被任何线程获得时，调用lock方法的线程将成功获的锁定并返回，将锁的保持计数设置为 1。
2.	如果当前线程已经获得该锁（重复获取锁），则将保持计数加 1，并且该方法立即返回。
3.	如果该锁已经被另一个线程获得，则出于线程调度的目的，将当前线程阻塞，并且在获得锁之前，该线程将一直处于休眠状态。

## Condition
参考文章：[怎么理解Condition](http://www.importnew.com/9281.html )

任何一个Java对象，都拥有一组监视器方法，主要包括wait()、notify()、notifyAll()方法，这些方法与synchronized关键字配合使用可以实现等待/通知机制。类似地，Condition接口也提供类似的Object的监视器的方法，主要包括await()、signal()、signalAll()方法，这些方法与Lock锁配合使用也可以实现等待/通知机制。

Lock 替代了 synchronized 方法和语句的使用，Condition 替代了 Object 监视器方法的使用。在Condition中，用await()替换wait()，用signal()替换notify()，用signalAll()替换notifyAll()。传统线程的通信方式，Condition都可以实现，这里注意，Condition是被绑定到Lock上的，要创建一个Lock的Condition必须用newCondition()方法。

相比Object实现的监视器方法，Condition接口的监视器方法具有一些Object所没有的特性：
1.	Condition接口可以支持多个等待队列，在前面已经提到一个Lock实例可以绑定多个Condition，所以自然可以支持多个等待队列了
2.	Condition接口支持响应中断，前面已经提到过
3.	Condition接口支持当前线程释放锁并进入等待状态到将来的某个时间，也就是支持定时功能

### 和Object监视器区别
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-3ecb0d335495ad96?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

Condition支持在await时不响应Thread的interrupt请求（awaitUninterruptibly方法）：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-f1fc7b5653b167b7?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

Condition支持的定时唤醒功能（不同于Object的带参wait方法，这里的定时指在指定日期唤醒）：
```java
    boolean awaitUntil(Date deadline) throws InterruptedException;
```
## 实现生产者-消费者
示例使用 jdk 1.8，用Consumer接口表示消费者，Supplier[]数组表示待生产的产品

### 关键成员变量
```java
    private final Lock lock = new ReentrantLock();
    
    //要生产的“货物”及其生产方式
    private final Supplier<T>[] suppliers;

    //消费者
    private final Consumer<T> consumer;

    //生产出的产品
    private T product;

    //生产者是否已经生产出一件产品
    private boolean isProducted = false;

    //条件：生产的产品还没有被消费者消费，需要等待消费者消费
    private Condition whenProductWaitConsume = lock.newCondition();

    //消费者是否已经消费了生产者生产的产品
    private boolean isConsumed = true;

    //条件：没有产品可以消费，需要等待生产者
    private Condition whenHaveProductToConsume = lock.newCondition();

    //生产者是否已经生产完所有的产品
    private boolean finished = false;

```

### 构造器
```java

    public SuppliersAndConsumerPlus(Consumer<T> consumer, Supplier<T>... suppliers) {
        this.suppliers = suppliers;
        this.consumer = consumer;
    }
```

### 生产过程
```java
 private void doProduct(Supplier<T> supplier) throws InterruptedException {
        lock.lock();
        try {
            while (!isConsumed) {
                P.out("supplier wait");
                // 等待生产条件：
                // 我发现刚生产的产品还没被消费掉，等到产品被消费之后提醒我
                whenProductWaitConsume.await();
            }

            // 收到可以生产的信号
            product = supplier.get();

            // 模拟生产耗时
            TimeUnit.SECONDS.sleep(2);
            P.out("+ " + product);

            isProducted = true;
            isConsumed = false;

            // 产品生产好了，可以消费了
            whenHaveProductToConsume.signal();
        } finally {
            lock.unlock();
        }
    }
```
遍历所有待生产的产品进行生产：
```java
    private void product() throws InterruptedException {
        // 遍历生产产品
        for (Supplier<T> supplier : suppliers) {
            doProduct(supplier);
        }
        finished = true;
    }
```


### 消费过程
```java
private void consume() throws InterruptedException {
        P.out("begin consumer");
        while (!finished) {
            lock.lock();
            try {
                while (!isProducted) {
                    P.out("consumer wait");
                    // 等待消费条件：
                    // 我发现现在还没有产品可以消费，所以等有产品可以消费了就提醒我
                    whenHaveProductToConsume.await();
                }

                // 收到可以消费的信号
                consumer.accept(product);

                // 模拟消费耗时
                TimeUnit.SECONDS.sleep(2);
                P.out("- " + product);

                isConsumed = true;
                isProducted = false;

                // 产品被消费了，可以继续生产了
                whenProductWaitConsume.signal();
            } finally {
                lock.unlock();
            }
        }
    }
```

### 启动生产者和消费者
```java
public void start() {
        // 开启消费者线程
        new Thread(() -> {
            try {
                consume();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }).start();

        //确保生产者在消费者之后启动
        try {
            TimeUnit.SECONDS.sleep(1);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        // 开启生产者线程
        new Thread(() -> {
            try {
                product();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }).start();

    }
```
### 测试
```java
    public static void main(String[] args) {

        @SuppressWarnings("unchecked")
        SuppliersAndConsumerPlus<String> consumer = new SuppliersAndConsumerPlus<>(
                String::getBytes,
                () -> "a",
                () -> "b",
                () -> "c",
                () -> "d");

        consumer.start();

    }
```

测试结果：
```bash
05/01/2018 09:47:53-794 begin consumer
05/01/2018 09:47:53-795 consumer wait
05/01/2018 09:47:56-762 + a
05/01/2018 09:47:56-762 supplier wait
05/01/2018 09:47:58-763 - a
05/01/2018 09:47:58-763 consumer wait
05/01/2018 09:48:00-763 + b
05/01/2018 09:48:00-763 supplier wait
05/01/2018 09:48:02-764 - b
05/01/2018 09:48:02-764 consumer wait
05/01/2018 09:48:04-764 + c
05/01/2018 09:48:04-764 supplier wait
05/01/2018 09:48:06-764 - c
05/01/2018 09:48:06-764 consumer wait
05/01/2018 09:48:08-765 + d
05/01/2018 09:48:10-765 - d

Process finished with exit code 0
```

代码已上传GitHub，你可以在[这里](https://github.com/DuanJiaNing/JavaStuff/tree/master/src/com/duan/javastuff/lock)找到
