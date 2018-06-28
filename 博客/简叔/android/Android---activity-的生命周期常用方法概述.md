![](http://upload-images.jianshu.io/upload_images/7460499-d9d99452609f771c.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
Android-activity的生命周期常用方法概述

#### activity生命周期

1. activity大致会经过如下四种状态
- 运行状态：当前activity位于前台，用户可见，可以获得焦点
- 暂停状态：其他activity位于前台，该activity依然可见，但不能获得焦点
- 停止状态：当前activity不可见，失去焦点

2. activity生命周期的常用方法
#####1.onCreate：
在activity的生命周期中这个方法是创建activity后第一个被系统调用的方
法。该方法只会调用一次。
`Bundle savedInstanceState`：Bundle类型的数据类似`Map`，以`key-value`的
形式存储数据。
>If the activity is being re-initialized after previously being shut down then this Bundle contains the data it most recently supplied in onSaveInstanceState(Bundle). Note: Otherwise it is null.<br><br>

如果活动在先前被关闭之后被重新初始化，那么该Bundle包含最近在
onSaveInstanceState（Bundle）中提供的数据。 注意：否则为null。

`savedInstanceState`参数的来源是activity生命周期的`onSaveInstanceState`方法
那<font color="red">活动在先前被关闭之后被重新初始化</font>指的是那些情况呢？

- 手机屏幕从竖屏切换成横屏或横屏切换成竖屏
- 程序从全屏显示切换为分屏显示的时候，回切也会调用
``` java
	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
```

##### API 21 加入的三个重载方法
API 21 为activity新增了一个属性`android:persistableMode`，将该属性设置
为`persistAcrossReboots`activity就具有持久化的能力，另外需要配合一个新
的Bundle`PersistableBundle `来保存数据。

- API 21 之前的`onRestoreInstanceState`,`onSaveInstanceState`,`onCreate`内的Bundle是在用户按下HOME键，返回键以及切换横竖屏等操作后回调`onSaveInstanceState`来操作交换的，而API 21 之后的这三个方法内的`PersistableBundle `则是针对**系统关机重启**的情况。[参考](http://blog.csdn.net/lincyang/article/details/45287599)
``` java
public void onSaveInstanceState(Bundle outState, PersistableBundle outPersistentState)

public void onRestoreInstanceState(Bundle savedInstanceState, PersistableBundle persistentState)

public void onCreate(Bundle savedInstanceState, PersistableBundle persistentState)
```
##### 2.onStart
>Called after onCreate — or after onRestart when the activity had been stopped, but is now again being displayed to the user. It will be followed by onResume.Derived classes must call through to the super class's implementation of this method. If they do not, an exception will be thrown.<br>

- 在onCreate方法后调用
- 当activity调用了onStop方法后（该activity已不可见）用户再次打开（回
- 到）该activity时调用onRestart，之后就会后调用onStart方法
- 在衍生类（子类）的该方法中必须调用父类的onStart()方法，否则就会
抛出异常
 
```  java
    @Override
    protected void onStart() {
        super.onStart();
    }
```
##### 3.onResume
>Called after onRestoreInstanceState, onRestart, or onPause, for your activity to start interacting with the user. This is a good place to begin animations, open exclusive-access devices (such as the camera), etc.Keep in mind that onResume is not the best indicator that your activity is visible to the user; a system window such as the keyguard may be in front. Use onWindowFocusChanged to know for certain that your activity is visible to the user (for example, to resume a game).Derived classes must call through to the super class's implementation of this method. If they do not, an exception will be thrown.

- 在onRestoreInstanceState, onRestart, or onPause后都有可能调用，调用此方法表示应用可以与用户进行交互。
- 在这里开始动画，打开独占设备（比如相机）等
- 注意此方法不是判断你的activity是否对用户可见的最佳指标，诸如软键盘，对话框等可能在你的activity的前面
- 使用onWindowFocusChanged 来确定activity对用户可见（比如在恢复游戏时）
- 在衍生类（子类）的该方法中必须调用父类的onResume()方法，否则就会抛出异常
 
``` java
    @Override
    protected void onResume() {
        super.onResume();
    }
```

##### 4.onPause
>Called as part of the activity lifecycle when an activity is going into the background, but has not (yet) been killed. The counterpart to onResume.When activity B is launched in front of activity A, this callback will be invoked on A. B will not be created until A's onPause returns, so be sure to not do anything lengthy here.This callback is mostly used for saving any persistent state the activity is editing, to present a "edit in place" model to the user and making sure nothing is lost if there are not enough resources to start the new activity without first killing this one. This is also a good place to do things like stop animations and other things that consume a noticeable amount of CPU in order to make the switch to the next activity as fast as possible, or to close resources that are exclusive access such as the camera.In situations where the system needs more memory it may kill paused processes to reclaim resources. Because of this, you should be sure that all of your state is saved by the time you return from this function. In general onSaveInstanceState is used to save per-instance state in the activity and this method is used to store global persistent data (in content providers, files, etc.)After receiving this call you will usually receive a following call to onStop (after the next activity has been resumed and displayed), however in some cases there will be a direct call back to onResume without going through the stopped state.Derived classes must call through to the super class's implementation of this method. If they do not, an exception will be thrown.<br>

- 当activity进入后台但activity没有（还没）被杀死时会调用此方法。
- 当activity A 处于可见状态，此时你打开了activity B ，这时activity A 的onPause方法会被调用。**activity B **将不会被创建直到A的onPause方法返回，所以不要在这里进行耗时操作。
- 这个回调方法大多用于保存正在编辑的任何持续性状态，向用户展现当前进行的编辑正常结束的状态，确保当出现**不杀死当前的activity是没有足够资源启动新的activity的**的情况时不会丢失用户正在编辑的数据。
- 在此停止动画和其他对CPU消耗较大的事情，或者关闭独占资源（相机等），以便更快的切换到另一个activity
- 在系统需要更多资源分配给其他更高优先级的应用时可能会杀死已暂停的进程以回收资源。因此要确保这个函数返回时要尽快的把程序的状态保存下来，而`onSaveInstanceState`就是负责这个的。所以可以覆写`onSaveInstanceState`来保存你自己的程序状态。
- 此方法调用后通常会调用onStop方法，但一些情况下也会直接调用onResume而不执行onStop进入停止状态
- 在衍生类（子类）的该方法中必须调用父类的onResume()方法，否则就会抛出异常
``` java
    @Override
    protected void onPause() {
        super.onPause();
    }
```
##### 5.onStop
>Called when you are no longer visible to the user. You will next receive either onRestart, onDestroy, or nothing, depending on later user activity.Derived classes must call through to the super class's implementation of this method. If they do not, an exception will be thrown.
 
- 当activity不可见时调用，在onPause一般会调用此方法（覆写了
onSaveInstanceState的话先调用onSaveInstanceState后调用此方法），有时也不会（onPause后调用了onResume）。
- 调用此方法后会调用onRestart（activity在此可见）、onDestroy（activity被销毁）或什么都不调用（应用进程被杀死），取决于后续用户的操作。
- 在衍生类（子类）的该方法中必须调用父类的onResume()方法，否则就会抛出异常
``` java
    @Override
    protected void onStop() {
        super.onStop();
    }
```
#####6.onDestroy
>Perform any final cleanup before an activity is destroyed. This can happen either because the activity is finishing (someone called finish on it, or because the system is temporarily destroying this instance of the activity to save space. You can distinguish between these two scenarios with the isFinishing method.Note: do not count on this method being called as a place for saving data! For example, if an activity is editing data in a content provider, those edits should be committed in either onPause or onSaveInstanceState, not here. This method is usually implemented to free resources like threads that are associated with an activity, so that a destroyed activity does not leave such things around while the rest of its application is still running. There are situations where the system will simply kill the activity's hosting process without calling this method (or any others) in it, so it should not be used to do things that are intended to remain around after the process goes away.Derived classes must call through to the super class's implementation of this method. If they do not, an exception will be thrown.

- 在activity被销毁时回调，只会调用一次
- 不要在该方法里保存数据，这些编辑应该在onPause或onSaveInstanceState中提交，而不是在这里。
-  通常该方法用于释放资源，结束开启的线程，确保应用还在运行时而该activity结束不留下一些属于该activity而不属于应用的东西。
-  系统也可能会简单的杀死应用进程而不调用此方法或其它的任何方法，因此此方法不应该被用来做那些不需要应用进程而运行的功能（如开启服务等）。
``` java
    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

```
<center>还有一张来自stromzhang公众号的图：</center><br>
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-9ff53c54e8770cda?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

参考：[Android Activity为什么要细化出onCreate、onStart、onResume、onPause、onStop、onDesdroy这么多方法让应用去重载？](http://blog.csdn.net/zhao_3546/article/details/12843477)

------------
