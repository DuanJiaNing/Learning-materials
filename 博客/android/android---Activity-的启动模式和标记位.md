![](http://upload-images.jianshu.io/upload_images/7460499-effeeb568ec5523c.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### android-Activity 的启动模式和标记位
##### Activity 启动过程涉及到的几个概念
>任务栈：
 > 任务栈又称 Task，顾名思义使用的是 **栈** 结构，具有先进后出的特点，栈中存放的是 Activity 组件的实例，我们每次启动一个 Activity （该 Activity 的 onCreate 方法将首次被调用），该 Activity 将被压入启动它的 Activity 所在的任务栈，或通过 Intent 指定`（addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)`在新的任务栈中创建该 Activity。任务栈有前台和后台之分，后台任务栈中的 Activity 处于暂停状态，用户可以通过切换应用（*Activity*）将后台任务栈调到前台。
####Activity 的 LaunchMode
1. standard 标准模式：
标准模式为系统默认启动模式。在该模式下每次启动 Activity 都会重新创建一个新的实例，不管这个实例是否已经存在。在这种模式下，谁启动了这个 Activity ，那么这个 Activity 就运行在启动它的那个 Activity 所在的任务栈中。
2. singleTop 栈顶复用模式：
- 在这种模式下，如果新 Activity 已经位于任务栈的栈顶，那么此 Activity 不会被重新创建	。
- 如果新 Activity 的实例已存在但不位于栈顶，那么新的 Activity 仍会重新创建。
-  onNewIntent 方法会被回调。
3. singleTask 栈内复用模式：
在这种模式下，只要 Activity 在一个栈中存在，那么多次启动该 Activity 都不会重新创建实例，同时系统会回调其 onNewIntent 方法。可以分为如下三种情况进行区分：
 - 要启动的 Activity A 所需的任务栈 S 不存在：
 系统先创建任务栈 S ，然后创建 A 并入栈到 S。
 - 要启动的 Activity A 所需的任务栈 S 存在，且任务栈 S 中不存在 A 的实例：
 系统将创建 A 的实例并入栈到 S。
  - 要启动的 Activity A 所需的任务栈 S 存在，且任务栈 S 中存在 A 的实例 ：
    1.  实例 A 位于栈顶：
    直接复用（*栈顶复用*） 
    2.  实例 A 不位于栈顶：
   此时 A 不会重新创建，同时将实例 A 切换到栈顶（*使实例 A 上面的所有 Activity 出栈*）     
4. singleInstance 单实例模式：
这是一种加强的 singleTask 模式，它具有 singleTask 的所有特性之外还有一个特性，那就是具有此种启动模式的 Activity 只能单独位于一个任务栈中。 
#### Activity 的 Flags
Activity 的 Flags 有很多，标记位的作用有很多，有的标记位可以设定 Activity 的启动模式，有的可以影响 Activity 的运行状态。
 
- FLAG_ACTIVITY_NEW_TASK
指定 Activity 以 singleTask 模式启动
-  FLAG_ACTIVITY_SINGLE_TOP
指定 Activity 以 singleTop 模式启动
-  FLAG_ACTIVITY_CLEAR_TOP
具备此标记为的 Activity 在启动时会将位于同一任务栈的所有位于它上面的 Activity 出栈，这个标记位一般会和 singleTask一起出现，singleTask 启动模式默认就具有此标记位的效果
-  FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS
具有这个标记的 Activity 不会出现在历史 Activity 列表中。

----
