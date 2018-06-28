![](http://upload-images.jianshu.io/upload_images/7460499-5a4969c3d7e8734d.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
#### 使用`HttpURLConnection`实现**多线程下载**
这个小程序是根据《疯狂Android讲义》*（第三版）*第13章 *Android网络应用*，13.3节 *使用HTTP访问网络*  扩展而来。
<br>
> 使用多线程下载文件可以更快地完成下载任务，因为客户端启动多条线程进行下载就意味着服务器也需要为该客户端提供相应的服务。假设服务器同时最多服务100个用户，在服务器中一个线程对应一个用户，100条线程在计算机内并发执行，也就是由CPU划分时间轮片流执行，如果A应用使用了99条线程下载文件，那么相当于占用了99个用户的资源，自然就拥有了较快的下载速度。<font size="-1">*不考虑网络因素*</font>

*实际上并不是客户端并发的下载线程越多下载速度就越快，因为当开启太多并发线程后，应用程序需要维护每条线程的开销，线程同步的开销，这些开销可能反而使下载速度减慢*

#### 先看看最终效果
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-71da681949423a58?imageMogr2/auto-orient/strip)
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-ee2dc0055262519f?imageMogr2/auto-orient/strip)
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-cf30c098011a1211?imageMogr2/auto-orient/strip)
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-6bd3b827e0e25578?imageMogr2/auto-orient/strip)
##### 1.这个小程序中负责开启多线程下载的核心类是：`DownloadUtil.java`
###### 这个类主要做了如下的事情：

- 创建URL对象（本程序使用的是`HttpURLConnection`）
- 获取URL指向资源的大小（由`HttpURLConnection.getContentLength()`方法完成）
- 在本地磁盘上创建一个与网络资源相同大小的空文件
（`RandomAccessFile.setLength()`）
- 计算每条线程应该下载网络资源的哪一部分
（*从哪个字节开始，到哪个字节结束*）
- 依次创建，启动多条线程来下载网络资源的指定部分

######下面来看具体代码：
1. 先来看构造函数
``` java
 public DownloadUtil(OnDownloadFinish complete) {
        this.downloadFinish = complete;
    }
```
`OnDownloadFinish`是下载完成后的回调
``` java
  public interface OnDownloadFinish {
        /**
         * 所有线程都结束后调用
         * @param file 下载好的文件
         */
        void onComplete(File file);
    }
```

2. 开启下载任务的入口方法
下载前的计算和开启线程下载都在这个方法中完成
``` java
	/**
     * 开始一次下载
     * @param sourcePath 目标URL
     * @param targetFilePath 目标保存路径
     * @param threadNumber 开启的线程数
     * @param fileName 保存的文件名
     * @throws IOException
     */
 public void start(@NonNull String sourcePath, @Nullable String targetFilePath, int threadNumber, @Nullable String fileName) throws IOException {
	 ....
 }
     
```

3. **创建HttpURLConnection对象：**
对于一次下载该对象指向的URL资源地址应是同一个，封装成一个方法：
该方法在两个地方被调用到
- 获得URL指向资源大小时。*获得大小后关闭该网络连接*
- 启动线程下载该线程对应的那部分资源时

``` java
 private HttpURLConnection getConnection() throws IOException {
        URL url = new URL(sourcePath);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setConnectTimeout(1000 * 5);
        conn.setRequestMethod("GET");
        conn.setRequestProperty("Accept", accept);
        conn.setRequestProperty("Accept-Language", "zh-CN");
        conn.setRequestProperty("Charset", "UTF-8");
        return conn;
    }
```

4. 下载前的计算和开启多个线程进行下载：
 获得大小->本地磁盘创建同大小的空文件->求得每一个线程应下载多少字节->开启线程进行下载
``` java
		HttpURLConnection conn = getConnection();
        fileSize = conn.getContentLength();
        conn.disconnect();

        RandomAccessFile file = new RandomAccessFile(targetFilePathAndName, "rw");
        file.setLength(fileSize);
        file.close();

        partSize = fileSize / threadNumber + 1;

        for (int i = 0; i < threadNumber; i++) {
            int startPos = i * partSize;
            threads[i] = new DownLoadThread(startPos);
            threads[i].start();
        }
```
5. 负责下载的线程类`private class` **`DownLoadThread`** `extends Thread`
来看run方法
在构造函数中写了该句`currentPart = new RandomAccessFile(targetFilePathAndName, "rw");`
>  `RandomAccessFile`: RandomAccessFile是用来访问那些保存数据记录的文件的，你可以用seek( )方法来访问记录，并进行读写了。这些记录的大小不必相同；但是其大小和位置必须是可知的。但是该类仅限于操作文件。——[Java RandomAccessFile用法 ](http://blog.csdn.net/akon_vm/article/details/7429245)<br>
> RandomAccessFile的对象包含一个记录指针，用于标识当前流的读写位置，这个位置可以向前移动，也可以向后移动。RandomAccessFile包含两个方法来操作文件记录指针。
- long getFilePoint():记录文件指针的当前位置。
- void seek(long pos):将文件记录指针定位到pos位置

----
在`finally`里判断当前还有几个线程还在下载，当没有线程在下载了（下载结束了），就回调`onComplete`方法。
``` java
		@Override
        public void run() {

            try {
                HttpURLConnection connection = getConnection();
                InputStream in = connection.getInputStream();
                skipFully(in, startPos);
                byte[] bytes = new byte[1024];
                int hasRead;
                while ((currentDownLoaded < partSize) && (hasRead = in.read(bytes)) > 0) {
                    currentPart.write(bytes, 0, hasRead);
                    currentDownLoaded += hasRead;
                }
                currentPart.close();
                in.close();
                connection.disconnect();

            } catch (MalformedURLException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                restTask--;
                if (restTask == 0)
                    downloadFinish.onComplete(new File(targetFilePathAndName));

            }

        }
```
其中用到一个跳过指定字节数的方法
> 由于最新的Android平台上调用InputStream的skip方法时，并不能总是准确地跳过指定的字节数，因此程序实现了一个skipFully方法。

参考此处：
[Java.IO.InputStream.skip() 错误（跳过字节数和预想的不等）](http://www.ablanxue.com/prone_7005_1.html)
``` java
/**
     * 从输入流中从起点开始跳过指定长度
     * @param in    输入流
     * @param bytes 要跳过的字节数
     * @throws IOException
     */
    public final void skipFully(InputStream in, long bytes) throws IOException {
        long len;
        while (bytes > 0) {
            len = in.skip(bytes);
            bytes -= len;
        }
```
好了，以上就时进行多线程下载的核心类，源码已上传到GitHub，你可以在这里找到：
<center>[DuanJiaNing/MultithreadDownload](https://github.com/DuanJiaNing/MultithreadDownload)</center>

##### 2. Activity部分
布局部分用到的控件主要有：
`TextView`,`EditText`,`ProgressBar`,`CardView`,`FloatingActionButton`和`ListView`.
###### 看起来是这样子的：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-739e22a646122746?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
解释一下：

- 最上面的`任务详情`:
1.当点击了`下载`之后当前正在下载任务的信息会显示在这里。
2.当已完成多个下载任务后，已下载的文件名会显示在`ListView`处，`ListView`的`onItemClick`时间会获得已下载文件对应的下载信息显示在这里。
- `进度`下面有个`ProgressBar`用于实时显示当前的下载进度
- `下载速度`处显示实时的下载速度（两行`byte/s kb/s`）
- `下载速度`**右边**也会显示实时的下载进度，对应`ProgressBar`的 `Max`和`当前进度`.
- `已下载`**上边**有个`TextView`显示文件保存的路径
- `FloatingActionButton`控制输入URL输入框，线程数输入框，下载按钮所在的`CardView`的显示和隐藏。
![](./Image_001.png)
###### 代码部分
1. 进入onCreate关联布局后其他几乎都在`init`方法里搞定了**`>.<`**
``` java
  @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_activity);

        init();

    }
```
2. `init()`方法里做了这么些事情：

* 初始化控件
* 初始化用于保存已下载文件名和对应任务信息的`ArrayList`
* `ListView`绑定`adapter`，item点击事件
* 从`anim`和`animator`中加载动画资源并绑定给`FloatingActionButton`和`CardView`,`FloatingActionButton`的点击事件（控制动画播放）
* `new`一个`DownloadUtil`实例并定义回调方法`onComplete`,在这里遇到了不少问题[^1] [^2]（亦可在源码注释中查看）
* `下载`按钮的点击事件，在点击事件里调用`DownloadUtil.start`方法，因为`start`方法里调用了`URLConnection.openConnection`方法，所以不能在**UI线程**里调用此方法。同时在这里开
启`Timer`实时更新UI。注意[^3]

3. 控制`CardView`动画的代码
``` java
 final AnimatorSet cvShow = (AnimatorSet) AnimatorInflater.loadAnimator(this, R.animator.input_show);
        final AnimatorSet cvHide = (AnimatorSet) AnimatorInflater.loadAnimator(this, R.animator.input_hide);
        cvShow.setTarget(cardView);
        cvHide.setTarget(cardView);

        input.setPivotX(100);//用ObjectAnimator实现旋转动画时用此方法指定旋转的原点
        final ObjectAnimator btShow = ObjectAnimator.ofFloat(input, "rotation", 0, 180).setDuration(1000);
        final ObjectAnimator btHide = ObjectAnimator.ofFloat(input, "rotation", 180, 0).setDuration(1000);
        btShow.setInterpolator(new OvershootInterpolator());
        btHide.setInterpolator(new DecelerateInterpolator());

        btShow.start();
        cvShow.start();

        input.setOnClickListener(new View.OnClickListener() {
            boolean hasCardViewShow = true;

            @Override
            public void onClick(View v) {
                if (hasCardViewShow) {
                    btHide.start();
                    cvHide.start();
                    hasCardViewShow = false;
                } else {
                    btShow.start();
                    cvShow.start();
                    hasCardViewShow = true;
                }
            }
        });
```
4. 下载完成后的回调和下载按钮的点击事件
代码有点多就直接上张图吧！
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-bcbb7e6359435889?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
5. 还有动画部分的代码

控制cardView隐藏
`/res/animator/input_hide.xml`
``` xml
<?xml version="1.0" encoding="utf-8"?>
<set xmlns:android="http://schemas.android.com/apk/res/android"
    android:ordering="together"
    >

    <objectAnimator
        android:duration="800"
        android:propertyName="translationY"
        android:interpolator="@android:anim/linear_interpolator"
        android:valueFrom="0.0f"
        android:valueTo="400.0f"
        android:valueType="floatType"
        />
    <objectAnimator
        android:duration="1000"
        android:propertyName="alpha"
        android:valueFrom="1.0f"
        android:valueTo="0.0f"
        android:valueType="floatType"
        />
</set>
```
控制cardView显示
`/res/animator/input_show.xml`
``` xml
<?xml version="1.0" encoding="utf-8"?>
<set xmlns:android="http://schemas.android.com/apk/res/android"
    android:ordering="together"
    >

    <objectAnimator
        android:duration="800"
        android:propertyName="translationY"
        android:interpolator="@android:anim/overshoot_interpolator"
        android:valueFrom="400f"
        android:valueTo="0.0f"
        android:valueType="floatType"
        />
    <objectAnimator
        android:duration="1000"
        android:propertyName="alpha"
        android:valueFrom="0.0f"
        android:valueTo="1.0f"
        android:valueType="floatType"
        />
</set>
```
6. `FloatingActionButton`的动画直接定义在java代码中了
``` java
input.setPivotX(100);//用ObjectAnimator实现旋转动画时用此方法指定旋转的原点
final ObjectAnimator btShow = ObjectAnimator.ofFloat(input, "rotation", 0, 180).setDuration(1000);
final ObjectAnimator btHide = ObjectAnimator.ofFloat(input, "rotation", 180, 0).setDuration(1000);
btShow.setInterpolator(new OvershootInterpolator());
btHide.setInterpolator(new DecelerateInterpolator());
```
7. 界面上有`EditText`所以一打开程序就会把软键盘调出来效果不好，所以在`style`里加了下面两句：
``` xml
<item name="android:configChanges">keyboardHidden|orientation</item>
<item name="android:windowSoftInputMode">stateHidden</item>
```
8. 要访问网络和读写磁盘，所以加入如下三条权限：
``` xml
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
```
-----

<center><font size="+2" color="#bbb">源码已上传到GitHub，你可以在这里找到</font></center>
<center><font size="+3">[DuanJiaNing/MultithreadDownload](https://github.com/DuanJiaNing/MultithreadDownload)</font></center>

[^1]: 定义全局的`MainActivity.this.file`来获得`onComplete`返回的`File`引用 或 在此处（`onComplete`内）定义一个final的File来获得file的引用。**不能使用第二种方法** ——假设使用第二种方法：当第一次下载（*打开程序进行的第一次下载任务*）成功回调该方法时file被赋值，赋值后dialog初始化，此时dialog持有file的引用，onComplete方法执结束时file被GC回收，但dialog初始时在onclick方法里使用了他的值，使每次调用dialog里onclick方法时使用的file都是第一次调用onComplete时的值，从而使File.reNameTo方法在调用两次之后就出错无法正常重命名。而每次调用editText.setText();时用的却是该次新的final File对象使对话框能正确显示文件名。因此应使用全局的file对象。

[^2]: `onComplete`方法在`DownloadUtil$DownLoadThread`中调用（<font size="+2">非`UI`线程</font>），因此在`onComplete`中更新UI要使用`Handler`,`Activity.runOnUiThread`或`View.post`。

[^3]: 只有在调用了`DownloadUtil.start`方法后，DownloadUtil的`getFileSize(),getThreadNumber(),getPartSize()`等`Getter`方法才会正确的取到值。

------
