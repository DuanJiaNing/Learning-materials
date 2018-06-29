开源一个功能相对齐全的本地音乐播放器

### 简述

从五月末就开始利用空余时间开发这款 app ，不知不觉三个月过去了。

App 名称：我的音乐，我给取了个别名：[Musicoco](https://github.com/DuanJiaNing/Musicoco)。

Android 手机本地音乐文件播放器。应用开启了单独的播放服务进程，在服务进程中控制音乐播放逻辑，目前已实现功能如下：通过耳机和通知栏快捷控制音乐播放、创建歌单、本地歌曲搜索、歌曲多选操作、记忆播放、自动切换到夜间模式、定时停止播放、应用主题自定义以及播放界面风格选择等功能。

### 应用截图

![](http://upload-images.jianshu.io/upload_images/7460499-87c6c0f2422d6c93.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 功能结构图
下图为应用的功能结构图，图有点繁杂，但把大部分功能都列了出来。
可进行如下归类：
1. 歌曲播放控制：播放/暂停、上/下曲切换、播放进度调整。（可从应用内、通知栏和耳机进行控制）
2. 歌曲操作：收藏/取消收藏、加入歌单/从歌单中移除、彻底删除、歌曲详情（前三个功能可选中多首歌曲后进行批量操作）
3. 歌单操作：新建、编辑、删除
4. 歌曲搜索：全部歌曲中搜索，特定歌单中搜索
5. 外观设置：播放界面风格、主题颜色、夜间/白天模式切换（自动切换）
6. 实用功能：定时停止播放、记忆播放、打开应用自动播放、图片墙
7. 额外功能：反馈、用户指南、应用信息、关于开发者、清除缓存

![image](http://upload-images.jianshu.io/upload_images/7460499-66cbbe4a4640a9a4.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 开发过程中遇到的部分难点

1.  aidl 进程间通信

通过在 AndroidManifest.xml 中为 Service 指定 process 属性就可以使服务运行于独立的进程中，应用中的服务为 `PlayService` ，应用启动时会以 `startService` 的方式启动服务，服务启动后会恢复上次播放歌曲（歌曲及其播放进度，前提为开启记忆播放），之后`Activity`再进行绑定(`bindService`)，客户端绑定服务之后主动获取服务端的当前歌曲并同步歌曲信息和播放状态，之后每一次播放歌曲改变、播放状态改变以及歌单改变服务端都会对绑定的客户端进行通知( 通过服务绑定者注册监听实现)。

问题出现在`Activity`第一次同步的时候，因为服务启动过程中有一个耗时操作（通过 ContentResolver 获得设备上的歌曲信息），之后进行当前歌曲恢复，完成之后会立即回调`songChanged`（服务端当前歌曲改变时回调的方法），这个过程是在 Service 的 onCreate 中完成的，这时 Activity 肯定不能而且也不能够成功绑定服务（因为 Service 还在 onCreate ，ServiceConnection 的 onServiceConnected 也不会回调，也就无法通过 IBinder 注册监听），但我却只在`songChanged`方法中等待回调以进行初始同步，却没意识到这个回调已经发生了，而此时服务正在启动，且此时不可能有任何客户端绑定到服务；这个错误的解决方法是在 Activity 成功绑定到服务后手动获取当前歌曲及播放状态进行同步。

2.  完全停止服务

`PlayService`服务会被两个 Activity 绑定，一个是主界面的 Activity（`MainActivity`），另一个为播放界面的 Activity（`PlayActivity`），停止时需要先让两个 Activity 解绑服务，之后让服务调用 `stopSelf` （通过发送广播实现）。

服务停止时需要释放 `MediaPlayer`；当时的问题是这样的，点击【退出】关闭应用，之后再次启动时，服务的 MediaPlayer 的 `reset` 方法抛出 `IllegalStateException`异常，这显然是因为 MediaPlayer 没有 release 导致的。我在应用【退出】操作的处理方法中是这样处理的，调用 Activity 的 finish 方法，此外在 `MainActivity` 和 `PlayActivity` 的 `onDestory`方法中解绑服务，`MainActivity`的 `onDestory` 最后还会发送广播通知服务 `stopSelf`，但这个异常还是一直抛出。调试好久之后终于找到错误，`PlayActivity`我设置了`singleInstance`，这意味着`PlayActivity`在单独的 activity栈 中，其他的 Activity 大都设置了 `singleTask`，而且能保证以【退出】按钮退出应用时 `MainActivity` 在其 activity栈中位于栈底，我天真的以为栈底 Activity 销毁时应用开启的 Activity 都会关闭，然而并没有， `PlayACtivity` 没有销毁更没有解绑服务，这导致 `PlayService`不能停止，MediaPlayer 的 release 也没有调用。解决方法就是手动调用 `PlayActivity` 的 `finish`。

3.  沉浸式状态栏

实现沉浸式状态栏的代码如下：
```
 if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            Window window = activity.getWindow();
            window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS
                    | WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);
            window.getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            window.setStatusBarColor(Color.TRANSPARENT);
        }
```
这部分代码能够让状态栏透明，在布局中配合`android:fitsSystemWindows="true"`就可以实现沉浸式状态栏，但当布局中有`NavigationView`时就出现了问题，当`NavigationView`打开时状态栏背景会变成半透明的黑色，而我期望的是完全透明的，半透明时的截图如下：

![](http://upload-images.jianshu.io/upload_images/7460499-3b8c2f6f17160b1b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

之后我发现将`android:fitsSystemWindows`置为 false 时状态栏会全透明，但标题栏会延伸到状态栏：

![](http://upload-images.jianshu.io/upload_images/7460499-20562af536910759.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

最后解决方法是置`android:fitsSystemWindows`为 false，同时为 Toolbar （布局中标题栏使用的是 DrawerLayout 嵌 CoordinatorLayout 嵌 AppBarLayout 嵌 CollapsingToolbarLayout 嵌 Toolbar 结构）加上状态栏高度的 padding。

![](http://upload-images.jianshu.io/upload_images/7460499-62bc35e1e24336ea.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

代码如下：
```
...

        toolbar = (Toolbar) findViewById(R.id.activity_main_toolbar);
        toolbar.post(new Runnable() {
            @Override
            public void run() {
                // fitsSystemWindows 为 false ，这里要增加 padding 填满状态栏
                toolbar.setPadding(0, Utils.getStatusBarHeight(MainActivity.this), 0, 0);
                ...
            }
        });
        setSupportActionBar(toolbar);

...
```
`Utils.getStatusBarHeight(Activity activity)`方法用于获得状态栏高度：

```
public static int getStatusBarHeight(Activity activity) {
        Rect frame = new Rect();
        activity.getWindow().getDecorView().getWindowVisibleDisplayFrame(frame);
        int statusBarHeight = frame.top;

        if (statusBarHeight <= 0) { // 有时会获取失败
            int resourceId = activity.getResources().getIdentifier("status_bar_height", "dimen", "android");
            if (resourceId > 0) {
                //根据资源ID获取响应的尺寸值
                statusBarHeight = activity.getResources().getDimensionPixelSize(resourceId);
            }
        }

        if (statusBarHeight <= 0) {
            statusBarHeight = 63;
        }

        return statusBarHeight;
    }
```

4.  SQLite 数据库事务和单例

数据库事务：
应用的数据库使用的是 SQLite，在对数据库进行操作时用到了事务，由于对数据库事务的不熟悉，导致写出了如下的代码：

```
            database.beginTransaction();
            ...
            database.endTransaction();
```

在结束事务之前如果没有调用`setTransactionSuccessful`标记事务成功，则操作会回滚，当时不知道还有这个操作，导致数据库操作始终被回滚，没有提交。所以提交事务之前要记得标记事务成功。

```
            database.beginTransaction();
            ...
            database.setTransactionSuccessful();
            database.endTransaction();
```

数据库操作类单例：
操作数据库的类使用了单例模式，持有 `ApplicationContext`，在用完数据库后调用 close 关闭数据库连接。单例的好处在于只存在一个对象，与 Application 共存亡，数据库操作类在构造方法中通过 `SQLiteOpenHelper`获得数据库连接，单例导致`SQLiteOpenHelper`的`getWritableDatabase`或`getReadableDatabase`只会调用一次，数据库操作类内的`SQLiteDatabase`对象也始终为同一个，即使在调用了`SQLiteDatabase`的 close 关闭了数据库连接，下次再次获得单例时`SQLiteDatabase`对象仍然是同一个，而且现在已经被关闭了，此时再使用它操作数据库就会得到异常：`java.lang.IllegalStateException: attempt to re-open an already-closed object: SQLiteDatabase: ....`，解决办法就是不使用单例  :-P。

5.  RecyclerView 被 NestedScrollView 嵌套时 RecyclerView 的复用

`RecyclerView ` 被 `NestedScrollView `嵌套时其复用机制失效，原因在于 RecyclerView   的高度被计算为所有 item 的高度之和，RecyclerView  会在第一次加载时就将所有 item 都加载，这在 item 较多时会导致 RecyclerView  加载卡顿， 这个问题在应用中体现为歌单详情界面中的歌曲列表，如下图所示，列表项包含歌曲基本信息及其专辑图片，当歌单中包含歌曲较多（>=40）时，界面加载就会出现明显卡顿。

![](http://upload-images.jianshu.io/upload_images/7460499-00539d2e792c7fd6.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

解决方法是手动为`RecyclerView ` 指定高度，计算方法如下（参照后面的歌单详情页截图更好理解）

```
    //计算 RecycleView 高度，否则无法复用 item
    private void calculateRecycleViewHeight() {
        ActionBar bar = ((AppCompatActivity) activity).getSupportActionBar();
        if (bar != null) {
            int actionH = bar.getHeight(); // actionBarHeight
            int randomCH = randomContainer.getHeight(); // randomPlayContainerHeight
            int statusH = Utils.getStatusBarHeight(activity); // statusBarHeight
            int screenHeight = Utils.getMetrics(activity).heightPixels;
            int height = screenHeight - actionH - statusH - randomCH;

            ViewGroup.LayoutParams params = songList.getLayoutParams();
            params.height = height;
            songList.setLayoutParams(params);
        }
    }
```


![](http://upload-images.jianshu.io/upload_images/7460499-e3cad9fa27d99cd5.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


暂时先总结这么多吧！

### 用到的库和开源自定义 View

1.   [wasabeef / glide-transformations](https://github.com/wasabeef/glide-transformations)：图片变换
2.   [akexorcist / Android-RoundCornerProgressBar](https://github.com/akexorcist/Android-RoundCornerProgressBar)：可修改颜色的圆形加载条
3.   [DuanJiaNing / MediaView](https://github.com/DuanJiaNing/MediaView)：播放/暂停，上/下曲按钮
4.   [ReactiveX / RxJava](https://github.com/ReactiveX/RxJava)
5.   [ReactiveX/RxAndroid](https://github.com/ReactiveX/RxAndroid)
6.   [bumptech/glide](https://github.com/bumptech/glide)
7.   [AnderWeb/discreteSeekBar](https://github.com/AnderWeb/discreteSeekBar)：进度条
8.   [DuanJiaNing/ColorPicker](https://github.com/DuanJiaNing/ColorPicker)：颜色选取器

部分功能尚未完善，还存在一些已知或未知的 bug，如果你想要改进客户端，或者发现了问题，可以提交[Issues](https://github.com/DuanJiaNing/Musicoco/issues)，或者直接使用应用的反馈功能向我提交你的意见或建议；当然，你也可以基于该代码修改并开发出属于自己的客户端。

应用还不能线上下载（我发布到应用宝了，可惜：*审核驳回，需提供 《信息网络传播视听节目许可证》，What the hell is this?*）

如果你想安装在自己手机上体验可在 GitHub 仓库 apk 目录下找到 app-release.apk 文件，或直接构建并运行项目。

GitHub 地址：[DuanJiaNing/Musicoco](https://github.com/DuanJiaNing/Musicoco)。

-----
