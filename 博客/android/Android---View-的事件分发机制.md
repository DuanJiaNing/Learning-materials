![](http://upload-images.jianshu.io/upload_images/7460499-a87571a83390dcbe.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### Android-View的事件分发机制

- View 的事件由 MotionEvent 类表示，MotionEvent 定义了大量的常量来表示用户的手指（鼠标、手写笔、轨迹球）在屏幕上的各种状态。View 的事件分发机制指的就是 View （ViewGroup、Button等）在复杂的层级关系里对 MotionEvent 事件的分配和处理规则。
- 事件序列：当一次手指触摸屏幕行为开始，往往后续会连续触发一连串的事件，如：DOWN -> ..MOVE.. -> UP。也可以说从 *ACTION_DOWN* 开始到  *ACTION_UP* 事件到达的过程为一次事件序列。
##### 一. MotionEvent
MotionEvent 类的定义如下：
`public final class MotionEvent extends InputEvent implements Parcelable`
该类继承了 InputEvent （抽象类，输入事件的表示类），同时该类是可序列化的。
###### 1.1 常见的事件状态：
- `ACTION_DOWN`
手指刚接触屏幕，按下的手势已经开始，此次事件序列的起始位置（坐标）被赋值。
-` ACTION_UP`
手指从屏幕松开的一瞬间，按下的手势结束，此次事件序列结束，最终的结束位置（坐标）决定。
- `ACTION_MOVE`
在接收到 ACTION_DOWN 之后，接收到 ACTION_UP 之前，在手指和屏幕保持接触的前提下手指的位置（坐标）发生了改变。
##### 1.2 默认的规则
<a id="rule"></a>
>规则 1
>>ViewGroup 默认不拦截任何事件。Android 源码中 ViewGroup 的 onInterceptTouchEvent 方法默认返回 false

> 规则 2
>> View 的 onTouchEvent 默认都会消耗事件（返回 true） ，除非他是不可点击的（clickable 、 longClickable 和 contextClickable 都为 false）。

>规则 3
>>onCLick 会发生的前提是当前 View 是可点击的，并且它收到了 DOWN 和 UP 事件。
##### 二.  触摸事件涉及到的主要方法
- `public boolean dispatchTouchEvent(MotionEvent event)`
事件分发逻辑处理的主要方法，如果触摸事件传递到当前 View 那么该方法一定会被调用，返回结果受当前 View 的 onTouchEvent 和下级 View 的 dispatchTouchEvent 方法的影响，表示是否消耗掉当前事件。
- `public boolean onInterceptTouchEvent(MotionEvent event)`
在 dispatchTouchEvent 方法中调用，用来判断是否拦截某个事件，如果当前 View 拦截了某个事件，那么在同一个事件序列中该方法不会再调用，返回结果表示是否拦截当前事件。 
- `public boolean onTouchEvent(MotionEvent event)`
在 dispatchTouchEvent 方法中调用，用来处理触摸事件，返回结果表示是否消耗当前事件，如果不消耗，则在同一事件序列中，当前 View 无法再次接收到事件。
##### 三. 事件分发过程
###### 3.1 ViewGroup 的 dispatchTouchEvent 方法
1. 对于一个根 ViewGroup 来说，点击事件产生后，首先会传递给它，这时它的` dispatchTouchEvent `就会被调用：
- 如果这个 ViewGroup 的 `onInterceptTouchEvent `方法返回 true就表示它要拦截当前事件，接着事件就会交给这个 ViewGroup 处理，即它的 `onTouchEvent`方法就会被调用
- 如果这个 ViewGroup 的 `onInterceptTouchEvent` 方法返回 false 就表示它不拦截当前事件，这时当前事件就会继续传递给它的子元素，接着子元素的 `dispatchTouchEvent `方法就会被调用，如此反复直到最终事件被处理。
2. 	参照源码加深理解
源码 `dispatchTouchEvent` 方法比较复杂，这里分段从前往后选取关键部分依次进行分析。

（1）判断当前 View（ViewGroup）是否拦截事件
``` java
		-------------ViewGroup#dispatchTouchEvent---------------
 public boolean dispatchTouchEvent(MotionEvent ev) {
 ...
// Check for interception.
            final boolean intercepted;
            if (actionMasked == MotionEvent.ACTION_DOWN
                    || mFirstTouchTarget != null) {
                final boolean disallowIntercept = (mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0;
                if (!disallowIntercept) {
                    intercepted = onInterceptTouchEvent(ev);
                    ev.setAction(action); // restore action in case it was changed
                } else {
                    intercepted = false;
                }
            } else {
                // There are no touch targets and this action is not an initial down
                // so this view group continues to intercept touches.
                intercepted = true;
            }
            ...
}
```
一句一句进行分析：
<center><font size="+1">**`if (actionMasked == MotionEvent.ACTION_DOWN || mFirstTouchTarget != null)`**:</font></center>
如果当前事件为 *ACTION_DOWN*，并且`mFirstTouchTarget `不为空，就进行后面的操作。那么`mFirstTouchTarget `是什么呢？
>根据 ViewGroup 的 onDispatchTouchEvent 方法后面部分得分析可知，当事件由 *ViewGroup*的子元素成功处理时（ViewGroup）没有进行拦截，mFirstTouchTarget 将被赋值，并指向其子元素，反过来，如果*ViewGroup*对事件进行拦截，mFirstTouchTarget != null 就不成立。

大家可能疑惑这句将`mFirstTouchTarget`是否为空作为判断条件，而`mFirstTouchTarget`的赋值却在这个`if()`判断之后，那`mFirstTouchTarget`不是一定为 null！
	`mFirstTouchTarget`作为全局变量，当`dispatchTouchEvent`**第一次**被调用时其值一定为空，而`actionMasked == MotionEvent.ACTION_DOWN`一定为 true ，这时，ViewGroup 的 `onInterceptTouchEvent`会被调用，如果其返回为 true，表示要拦截（那么 intercepted 为 true，mFirstTouchTarget 将不被赋值为空），那么此次的 *ACTION_DOWN*事件就被拦截，*ACTION_DOWN*是一次事件序列的开始，那么当此次事件序列的下一个事件到达调用`dispatchTouchEvent`方法并运行到`if()`时，该`if()`将为 false（此时 `actionMasked != MotionEvent.ACTION_DOWN`且`mFirstTouchTarget  == null`），则 `intercepted = true`，这就得出一个结论：
>结论 1 
>>某个 View 一旦决定拦截，那么这一事件序列都只能由它来处理（如果事件序列能够传递给他的话），并且它的 onInterceptTouchEvent 不会再被调用。

<center><font size="+1">**`final boolean disallowIntercept = (mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0;if (!disallowIntercept) {...`**</font></center>
这句中使用到了一个标记位`FLAG_DISALLOW_INTERCEPT`，那么这个标记位是否启用是由谁决定、怎么决定的呢？
这里我们讨论的是 ViewGroup 的	`dispatchTouchEvent` 方法，`dispatchTouchEvent` 方法如果不拦截事件，那么 ViewGroup 就会将事件传递给它的子 View，这时子 View 可以通过其所在容器（父视图 ViewGroup）的引用调用 ViewGroup 的 `requestDisallowInterceptTouchEvent(boolean disallowIntercept)`方法改变 ViewGroup 的行为（启用或停用`FLAG_DISALLOW_INTERCEPT`标记位），使 ViewGroup 不再拦截除 ACTION_DOWN 以外的其它事件。

为什么说除了 ACTION_DOWN 以外的事件呢？
>这是因为 ViewGroup 在分发事件时，如果是 ACTION_DOWN 事件就会重置 `FLAG_DISALLOW_INTERCEPT`标记位，使该标记位失效，即当面对 ACTION_DOWN 事件时，ViewGroup 总会调用自己的 `onInterceptTouchEvent`方法来询问自己是否要拦截事件。这一点在源码中也有体现：
```java
// Handle an initial down.
            if (actionMasked == MotionEvent.ACTION_DOWN) {
                // Throw away all previous state when starting a new touch gesture.
                // The framework may have dropped the up or cancel event for the previous gesture
                // due to an app switch, ANR, or some other state change.
                cancelAndClearTouchTargets(ev);
                resetTouchState();
            }
```

>结论 2 
>>事件传递是由外向内的，即事件总是先传递给父元素，然后再由父元素分发给子元素，通过 requestDisallowInterceptTouchEvent 方法可以在子元素中干预父元素的事件分发过程，但是 ACTION_DOWN 事件除外。

（2）ViewGroup 不拦截事件，将事件分发给子 View 

```java
       -------------ViewGroup#dispatchTouchEvent---------------
public boolean dispatchTouchEvent(MotionEvent ev) {
...
						final View[] children = mChildren;
                        for (int i = childrenCount - 1; i >= 0; i--) {
                            final int childIndex = getAndVerifyPreorderedIndex(
                                    childrenCount, i, customOrder);
                            final View child = getAndVerifyPreorderedView(
                                    preorderedList, children, childIndex);

                            // If there is a view that has accessibility focus we want it
                            // to get the event first and if not handled we will perform a
                            // normal dispatch. We may do a double iteration but this is
                            // safer given the timeframe.
                            if (childWithAccessibilityFocus != null) {
                                if (childWithAccessibilityFocus != child) {
                                    continue;
                                }
                                childWithAccessibilityFocus = null;
                                i = childrenCount - 1;
                            }

                            if (!canViewReceivePointerEvents(child)
                                    || !isTransformedTouchPointInView(x, y, child, null)) {
                                ev.setTargetAccessibilityFocus(false);
                                continue;
                            }

                            newTouchTarget = getTouchTarget(child);
                            if (newTouchTarget != null) {
                                // Child is already receiving touch within its bounds.
                                // Give it the new pointer in addition to the ones it is handling.
                                newTouchTarget.pointerIdBits |= idBitsToAssign;
                                break;
                            }

                            resetCancelNextUpFlag(child);
                            if (dispatchTransformedTouchEvent(ev, false, child, idBitsToAssign)) {
                                // Child wants to receive touch within its bounds.
                                mLastTouchDownTime = ev.getDownTime();
                                if (preorderedList != null) {
                                    // childIndex points into presorted list, find original index
                                    for (int j = 0; j < childrenCount; j++) {
                                        if (children[childIndex] == mChildren[j]) {
                                            mLastTouchDownIndex = j;
                                            break;
                                        }
                                    }
                                } else {
                                    mLastTouchDownIndex = childIndex;
                                }
                                mLastTouchDownX = ev.getX();
                                mLastTouchDownY = ev.getY();
                                newTouchTarget = addTouchTarget(child, idBitsToAssign);
                                alreadyDispatchedToNewTouchTarget = true;
                                break;
                            }
                   ...
}
```
<center><font size="+1">`for (int i = childrenCount - 1; i >= 0; i--) {...`</font>
</center>
将事件分发给子 View 的过程通过遍历每一个子 View，判断子 View是否能接收到事件，能就调用其 `dispatchTouchEvent`方法，若子 View 的 `dispatchTouchEvent`方法返回 false ，表示子 View 未消耗事件，则继续循环；如果子 View 的 `dispatchTouchEvent`方法返回 true，表示子 View 消耗了事件，那么`mFirstTouchTarget`将被赋值，同时结束循环。
- 如何判断子 View 是否能接收到事件：
子元素是否在播放动画
点击事件的坐标是否落在子元素的区域内
```
...
if (childWithAccessibilityFocus != null) {
	...
}
if (!canViewReceivePointerEvents(child) || !isTransformedTouchPointInView(x, y, child, null)) {
	...
}
...
```
- `mFirstTouchTarget`的赋值：对应代码为：
```java
newTouchTarget = addTouchTarget(child, idBitsToAssign);
alreadyDispatchedToNewTouchTarget = true;
break;
```
具体的赋值在`addTouchTarget`方法内部。
- 遍历所有子元素都没有被处理：这包含两种情况，第一种为 ViewGroup 没有子元素，第二种是子元素处理了点击事件，但 子元素的 `dispatchTouchEvent`方法返回了false（这一般是因为子元素在`onTouchEvent`方法中返回了false），此时 ViewGroup 将自己处理事件。
结论：
<a id="result3"></a>
>结论 3 
>>某个 View 一旦开始处理事件，如果它不消耗 ACTION_DOWN 事件（onTouchEvent返回false），那么同一事件序列中的其他事件都不会再交给它来处理，并且事件将重新交由它的父元素去处理，即父元素的 onTouchEvent 会被调用。意思就是事件一旦交给一个 View 来处理，那么它就必须消耗掉，否则同一事件序列中剩下的事件就不再交给它来处理。


 这个结论可以结合代码来理解：
**`某个 View 一旦开始处理事件`**：在开始循环前各个关键变量的值应该是这样的：
1.`mFirstTouchTarget`一定为空
2.`intercepted`一定为 false
3.可以推出`actionMasked == MOtionEvent.ACTION_DOWN`（这里参照 3.1 的部分）。

 之后遍历子 View 开始，在确定了**某一个**子 View *CV* 能接收到事件后，调用*CV*的`dispatchTouchEvent`方法（`dispatchTransformedTouchEvent`方法内部）：
 
 - 若`dispatchTouchEvent`返回 true ，表示*CV*消耗了**ACTION_DOWN**事件，这时对`mFirstTouchTarget`赋值，使其指向*CV*，并跳出循环。当此次事件序列的下一个事件到达，假设为 ACTION_MOVE，此时`mFirstTouchTarget != null`成立，如果*CV*调用了`requestDisallowInterceptTouchEvent`方法启用`FLAG_DISALLOW_INTERCEPT`标记位（`disallowIntercept 为 true`），则 `intercepted`为 false，若`FLAG_DISALLOW_INTERCEPT`标记位没有被启用，那么 ViewGroup 的 `onInterceptTouchEvent`再次调用，参考 <a href="#rule">规则 1 </a>可以知道，一般情况下`intercepted`也会为 false，那么 **同一事件序列中的其他事件都会交给它处理**。
 - 若`dispatchTouchEvent`返回 false，即 *CV* 不消耗 ACTION_DOWN 事件（onTouchEvent返回false）,此时会继续往下遍历子 View，如果遍历结束都没有被处理，那么 ViewGroup 将自己处理该事件，即**父元素的 onTouchEvent 会被调用**。
 
###### 3.2 View 的 dispatchTouchEvent 方法
View 的`dispatchTouchEvent `方法要简单一些，这里的 View 不包括 ViewGroup 。
>View（不包括 ViewGroup）做为一个单独的元素，它没有子元素无法向下传递事件，所以只能自己处理事件。

（一） 事件传递到了某一个具体的 View 那就表明该 View 能接收到触摸事件，传递过来的第一个事件一定是此次事件序列的 `ACTION_DOWN` 事件，如果 View 消耗了 `ACTION_DOWN` 事件（返回true），那么该事件序列的后续事件都会传递给他，如果没消耗 `ACTION_DOWN` ，那么它将无法收到后续事件。参考  <a href="#result3">结论 3 </a>

如果 View 只想处理 `ACTION_DOWN` 事件，而不处理其他事件，那可以覆写 `dispatchTouchEvent`方法或通过设置监听器实现（`setOnTouchListener(OnTouchListener l)`），在方法中加入如下判断即可：
```java
...
if(event.getAction() == MotionEvent.ACTION_DOWN)
	return false;
...
```
（二） 内部监听和外部监听 
```java
-------------View#dispatchTouchEvent---------------
 public boolean dispatchTouchEvent(MotionEvent event) {
...
        if (onFilterTouchEventForSecurity(event)) {
            if ((mViewFlags & ENABLED_MASK) == ENABLED && handleScrollBarDragging(event)) {
                result = true;
            }
            //noinspection SimplifiableIfStatement
            ListenerInfo li = mListenerInfo;
            if (li != null && li.mOnTouchListener != null
                    && (mViewFlags & ENABLED_MASK) == ENABLED
                    && li.mOnTouchListener.onTouch(this, event)) {
                result = true;
            }

            if (!result && onTouchEvent(event)) {
                result = true;
            }
        }

...
        return result;
    }
```
>从源码可以看出，首先会判断有没有设置 OnTouchListener ，如果有，并且 OnTouchListener 中的 onTouch 方法返回 true，那么 onTouchEvent 就不会被调用，可见 OnTouchListener 的优先级高于 OnTouchEvent，这样做的好处是方便在外界处理触摸事件。

所以如果想屏蔽 View 默认的触摸事件处理只需为 View 设置监听器并返回 true 即可。
```java
view.setOnTouchListener(new View.OnTouchListener() {
	  @Override
      public boolean onTouch(View v, MotionEvent event) {
          return true;
      }
 });
```

（三）View 的 onTouchEvent 对 DISEABLED 的处理
```java
   public boolean onTouchEvent(MotionEvent event) {
       ...
        if ((viewFlags & ENABLED_MASK) == DISABLED) {
            if (action == MotionEvent.ACTION_UP && (mPrivateFlags & PFLAG_PRESSED) != 0) {
                setPressed(false);
            }
            // A disabled view that is clickable still consumes the touch
            // events, it just doesn't respond to them.
            return (((viewFlags & CLICKABLE) == CLICKABLE
                    || (viewFlags & LONG_CLICKABLE) == LONG_CLICKABLE)
                    || (viewFlags & CONTEXT_CLICKABLE) == CONTEXT_CLICKABLE);
        }
	  ...
        return false;
    }
```
>从源码可看出，当 View 处于不可用状态时，只要 CLICKABLE ，LONG_CLICKABLE 和 CONTEXT_CLICKABLE  有一者为 true，onTouchEvent 就会返回 true，即消耗事件。

这里有个`CONTEXT_CLICKABLE`，这个常量指的又是什么呢？
`CONTEXT_CLICKABLE`是 Android SDK 23 （Android 6.0）加入的，表示触控笔上下文单击（触控笔按钮）和鼠标右键单击是否可用。为其设置监听器时应使用 onContextClick 代替 onStylusButtonPress。

（四）View 的 onTouchEvent 对点击事件的实现
```java
   public boolean onTouchEvent(MotionEvent event) {
       ...
         if (((viewFlags & CLICKABLE) == CLICKABLE ||
                (viewFlags & LONG_CLICKABLE) == LONG_CLICKABLE) ||
                (viewFlags & CONTEXT_CLICKABLE) == CONTEXT_CLICKABLE) {
          switch (action) {
          case MotionEvent.ACTION_UP:
           ...
                            if (!focusTaken) {
                                // Use a Runnable and post this rather than calling
                                // performClick directly. This lets other visual state
                                // of the view update before click actions start.
                                if (mPerformClick == null) {
                                    mPerformClick = new PerformClick();
                                }
                                if (!post(mPerformClick)) {
                                    performClick();
                                }
                            }
	                       }
	  ...
        return false;
    }
```
>当 ACTION_UP 事件到达时，会触发 performClick 方法，如果 View 设置了 OnClickListener，那么 performClick 内部会调用它的 onClick 方法。 

注释中说明了要使用 post 方法通过 Handler 执行 onClick，这让我们能够在 onClick 执行之前看到 View 的状态更新。

（五）CLICKABLE ，LONG_CLICKABLE 和 CONTEXT_CLICKABLE  的设置
>View 的 LONG_CLICKABLE 属性默认为 false ，而 CLICKABLE 属性是否为 false 和具体的 View 相关，确切来说是可点击的 View 的 CLICKABLE 为 true ，不可点击的 View 的 CLICKABLE 为 false，比如 Button 是可点击的，而 TextView 是不可点击 的。通过 setClickable 、 setLongClickable 和 setContextClickable 可修改三者的值。另外，setOnClickListener、setOnLongClickListener 和 setOnContextClickListener 方法内部会自动将 View 的对应属性的 XXXable 值改为 true。

文章大部分摘抄自《Android 开发艺术探索》，第三章 3.4 节 —— View的事件分发机制，加上部分自己的理解，可能有错误，欢迎指正。

----
