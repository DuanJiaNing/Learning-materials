## 文件锁 FileLock
Jdk 1.4 引入文件加锁机制，它允许我们**同步访问某个作为共享资源的文件**。不过，竞争同一个文件的两个线程可能在不同的JVM上；或者一个未java线程，另一个为操作系统中其他的每个本地线程。**文件锁对其它的操作系统进程是可见的因为java的文件锁是直接映射到了本地操作系统的加锁工具。**

![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-f0e8ac9f95aa08b9?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### tryLcok和lock

tryLock()是非阻塞式的，它设法获取锁，但是如果不能获得（但其他一些进程已经持有相同的锁，并且不共享时），它将直接从方法调用返回。

Lock()则是阻塞式的，它将阻塞线程直至锁可以获得，或调用lock的线程中断，或调用lock的通道关闭。使用release()方法可以释放锁。

### 对文件的一部分内容上锁
可以使用如下的方法对文件的一部分内容上锁：
`lock(long position,long size,boolean shared);`
`tryLock(long position,long size,boolean shared)`
其中，加锁的区域由position-size决定，第三个参数指定是否使用共享锁。

尽管无参数的加锁方法根据文件尺寸的变化而变化，但是**具有固定尺寸的锁不随文件尺寸的变化而变化**，如果你获得position到position+size部分的锁，当文件增大，超出position+size时，那么position+size之外的部分不会被锁定。**无参数的加锁方法会对整个文件进行加锁，甚至文件变大后也是如此。**

独占锁和共享锁的使用需要底层操作系统支持，如果操作系统不支持共享锁并为每一个请求都创建一个锁，那么它就会使用独占锁。可以通过FileLock.isShared()进行查询。

## 例：锁住文件并修改内容
配合MappedByteBuffer对文件的不同部分加锁并用多个线程修改文件内容。
 
```

    private static FileChannel channel;

    public static void main(String[] args) throws IOException {
        final int LENGTH = 1 << 10; // 1k
        channel = new RandomAccessFile(to, "rw").getChannel();

        // 映射文件 0 - 1kb 部分
        MappedByteBuffer mb = channel.map(FileChannel.MapMode.READ_WRITE, 0, LENGTH);
        for (int i = 0; i < LENGTH; i++) {
            mb.put((byte) 'A');
        }
        // 锁住并修改文件的 0 - 1/3 部分
        new LockAndModify(mb, 0, LENGTH / 3);

        // 锁住并修改文件的 1/2 - 3/4 部分
        new LockAndModify(mb, LENGTH / 2, LENGTH / 2 + LENGTH / 4);
    }

    private static class LockAndModify extends Thread {
        private ByteBuffer buffer;
        private int start, end;

        LockAndModify(ByteBuffer buffer, int start, int end) {
            this.buffer = buffer;
            this.start = start;
            this.end = end;
            buffer.limit(end);
            buffer.position(start);

            // 基于已有buffer创建新的buffer
            this.buffer = buffer.slice();
            start();
        }

        @Override
        public void run() {
            try {

                // 阻塞锁
                FileLock lock = channel.lock(start, end, false);
                P.out("locked: " + start + " " + end);
                while (buffer.position() < buffer.limit() - 1) {
                    // 将大写字母 A 变换为小写的 a
                    byte b = (byte) (buffer.get() + 32);
                    buffer.put(b);
                }
                lock.release();
                P.out("release: " + start + " " + end);

            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


```

修改的文件：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-63e35eec5e049af3?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
 
输出：
17/01/2018 14:39:38-320 locked: 512 768
17/01/2018 14:39:38-320 locked: 0 341
17/01/2018 14:39:38-321 release: 0 341
17/01/2018 14:39:38-321 release: 512 768


如果有JVM，文件锁会被自动释放，或者关闭加锁的通道。不过更应该主动显示的为FileLock对象调用release方法释放锁。

---
文章大部分内容摘抄自《Thinking in java》第18章：18.10 新I/O

