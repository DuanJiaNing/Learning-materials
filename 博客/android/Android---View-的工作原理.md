![](http://upload-images.jianshu.io/upload_images/7460499-5dcc1c4fdf27fcb4.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
### Android-View 的工作原理
>View 的工作流程主要指的是 measure、layout、draw 这三大流程，即测量、布局和绘制，其中 measure 确定 View 的测量宽/ 高，layout 确定 View 的最终宽/高和四个顶点的位置，而 draw 则将 View 绘制到屏幕。

在介绍 View 的三大流程之前，需要先了解 ViewRoot、DecorView 和 MeasureSpec 的基本概念，才能更好的理解 View 的 measure、layout、draw过程。
<br>
#### 1 ViewRoot 和 DecorView
>- ViewRoot 对应于 ViewRootImpl 类，它是连接 WindowManager 和 DecorView 的纽带，View 的三大流程均是通过 ViewRoot 来完成的。 
<br>
>- View 的绘制流程是从 ViewRoot 的 performTraversals 方法开始的，它经过 measure、layout 和 draw 三个过程才能最终将一个 View 绘制出来，
<br>
>- measure 决定了 View 的宽和高，measure 完成之后，可以通过 getMeasureHeight/Width 来获得 View 的测量宽和高，在几乎所有情况下它都是与 View 的最终宽高相等的（getHeight/Width）。 layout 过程决定了 View 的四个顶点和最终宽高，完成以后就可以通过 getTop/Left/Bottom/Right 来获得其四个顶点相对父容器的坐标。只有 draw 完成以后 View 的内容才会呈现到屏幕上。
<br>
>- DecorView 做为顶级 View ，一般情况其内部会包含一个竖直方向的 LinearLayout，该 LinearLayout 分为两部分，上面部分为 标题栏，下面为内容栏。内容栏为一个 id 为 android.R.id.content 的 FrameLayout，而平时开发使用 setContentView 时就是将 View 添加到这个 FrameLayout 中。
>因此要在 Activity 中获得我们设置的 View，可通过如下方式获得：
>`View contentView = 
((ViewGroup)getWindow().getDecorView().findViewbyId(android.R.Id.content)).getChildAt(0);`

#### 2 MeasureSpec
>MeasureSpec 代表一个 32 为的 int 值，高 2 位代表 SpecMode，低 30 位表示 SpecSize，SpecMode 指测量模式，SpecSize 指某种测量模式下的规格大小。

##### 2.1 SpecMode 有三种
- UNSPECIFIED 父容器不对 View 有任何限制，要多大给多大，这种情况一般用于系统内部，表示一种测量的状态。
<br>
- EXACTLY 父容器已经测出 View 所需的精确大小，这个时候 View 的最终大小就是 SpecSize 的值。**它对应于 LayoutParams 中的 match_parent 和具体的数值这两种模式**。
<br>
- AT_MOST 父容器指定一个可用的最大大小即 SpecSize，View 的大小不能大于这个值，具体是什么要看不同 VIew 的具体实现。**它对应于 LayoutParams 中的 warp_content**。

##### 2.2 MeasureSpec 和 LayoutParams 的对应关系
MeasureSpec 的值是我们无法直接控制的，但我们可以通过给 View 设置 LayoutParams 来间接修改 MeasureSpec 的值。
>在 View 测量的时候，系统会将 LayoutParams 在父容器的约束下转换为对应的 MeasureSpec，然后根据这个 MeasureSpec 测量出 View 的宽高。需要注意的是父容器传给待测 View（子 View）的 MeasureSpec 的值由 子 View 的 LayoutParams 和父容器（父容器的 MeasureSpec）共同决定。

子 View 的 MeasureSpec 赋值规则在**`ViewGroup#getChildMeasureSpec(int spec, int padding, int childDimension)`**方法中定义：
该方法的调用可在 ViewGroup 的 `measureChild`和`measureChildWithMargins`方法中找到。

赋值规则可总结为如下表格：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-57d2faf9029c8b76?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
<br>由上表可以清晰的看出子 View 的 MeasureSpec 的确定规则，比如当父容器的 MeasureSpec 的 SpecMode 为 EXACTLY： 

- 子 View 的 LayoutParams (android:width/height)为具体数值（如20dp）时，子 View 的`onMeasure(int widthMeasureSpec, int heightMeasureSpec)`方法中得到的 MeasureSpec（widthMeasureSpec 或 heightMeasureSpec）的 SpecMode 将为 EXACTLY，SpecSize 为 20dp；
<br>
- 子 View 的 LayoutParams (android:width/height)为 match_parent 时，子 View 的 MeasureSpec 的 SpecMode 将为 EXACTLY，SpecSize 为父容器的大小；
<br>
- 子 View 的 LayoutParams (android:width/height)为 warp_content 时，子 View 的 MeasureSpec 的 SpecMode 将为 AT_MOST，SpecSiz 为父容器的大小。
<br>
- UNSPECIFIED 主要用于系统内部多次 Measure 的情形，一般来说，不需要关注。
##### 2.3 MeasureSpec 的传递
父容器（ViewGroup）传递给子 View 的 MeasureSpec 的值通过**`getChildMeasureSpec`**方法确定，父容器将多次调用该方法以分别求得子 View 的`widthMeasureSpec`和`widthMeasureSpec`，然后调用子 View 的`measure`方法，最终将子 View 的 MeasureSpec 传递到`onMeasure`方法中完成子 View 的测量。

#### 3 measure 过程
>measure 过程分情况来看，如果只是一个原始的 View（继承体系中没有 ViewGroup），那么通过 measure 方法就完成其测量过程，如果是 ViewGroup ，除了完成自己的测量过程外，还要遍历去调用子元素的 measure 方法，各个子元素再递归去执行这个流程，完成测量。

##### 3.1 View 的 measure 过程
View 的 measure 过程由其 measure 方法控制，measure 方法内部会调用 onMeasure 方法完成具体的测量。
<center>**View # onMeasure(int widthMeasureSpec, int heightMeasureSpec)**</center>
```java
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        setMeasuredDimension(getDefaultSize(getSuggestedMinimumWidth(), widthMeasureSpec),
                getDefaultSize(getSuggestedMinimumHeight(), heightMeasureSpec));
    }
```

<center>**View # getDefaultSize(int size, int measureSpec)**</center>
```java
 public static int getDefaultSize(int size, int measureSpec) {
        int result = size;
        int specMode = MeasureSpec.getMode(measureSpec);
        int specSize = MeasureSpec.getSize(measureSpec);

        switch (specMode) {
        case MeasureSpec.UNSPECIFIED:
            result = size;
            break;
        case MeasureSpec.AT_MOST:
        case MeasureSpec.EXACTLY:
            result = specSize;
            break;
        }
        return result;
    }
```
对 onMeasure 方法内调用的方法进行解释：

- getSuggestedMinimumWidth / Height()：返回视图应使用的建议最小宽度/高度
内部逻辑：如果 View 没有设置背景，那么返回 android:minWidth / minHeight 属性指定的值，这个值可以为 0 ；如果设置了背景，则返回 android:minWidth / minHeight 和背景的最小宽度/高度这两者中的最大值。
<br>
- getDefaultSize(int size, int measureSpec)：该方法内部逻辑也比较简单，UNSPECIFIED 的情况我们不需要关注，在 AT_MOST 或 EXACTLY 模式下返回值 即为 width / heightMeasureSpec 的 SpecSize。
<br>
-  setMeasuredDimension(int measuredWidth, int measuredHeight)：该方法返回时我们就可以在 onLayout 中通过 getMeasureWidth 方法获得测量宽高了。
##### 3.2 ViewGroup 的 measure 过程
 ViewGroup 除了完成自己的测量过程外，还要遍历去调用子元素的 measure 方法，各个子元素再递归去执行这个流程。
 >和 View 不同的是，ViewGroup 是一个抽象类，因此它没有重写 View 的 onMeasure 方法，但它提供了一个叫 measureChildren 的方法。

 <center>**ViewGroup # measureChildren(int widthMeasureSpec, int heightMeasureSpec)**</center>
```java
  protected void measureChildren(int widthMeasureSpec, int heightMeasureSpec) {
        final int size = mChildrenCount;
        final View[] children = mChildren;
        for (int i = 0; i < size; ++i) {
            final View child = children[i];
            if ((child.mViewFlags & VISIBILITY_MASK) != GONE) {
                measureChild(child, widthMeasureSpec, heightMeasureSpec);
            }
        }
    }
```
 <center>**ViewGroup # measureChild(View child, int parentWidthMeasureSpec,**
            **int parentHeightMeasureSpec)**
```java
    protected void measureChild(View child, int parentWidthMeasureSpec,
            int parentHeightMeasureSpec) {
        final LayoutParams lp = child.getLayoutParams();

        final int childWidthMeasureSpec = getChildMeasureSpec(parentWidthMeasureSpec,
                mPaddingLeft + mPaddingRight, lp.width);
        final int childHeightMeasureSpec = getChildMeasureSpec(parentHeightMeasureSpec,
                mPaddingTop + mPaddingBottom, lp.height);

        child.measure(childWidthMeasureSpec, childHeightMeasureSpec);
    }
```
measureChildren 方法会遍历子 View，并对子 View 执行测量流程，子 View 的测量流程调用在 measureChild 方法中开启， measureChild 方法内部会调用 getChildMeasureSpec 方法获得子 View 的 MeasureSpec ，**然后调用子 View 的 measure 方法**(如果子 View 为 ViewGroup 则递归回到 ViewGroup 的 measure 过程，如果子 View 为原始的 View，则到了 View 的 measure 过程，这一次递归即将结束)。

ViewGroup 作为抽象类并没有定义其测量的具体过程（抽象类只能实例化其子类，子类必须重写从 View 中继承来的 onMeasure 方法），比如 Linearlayout、RelativeLayout、FrameLayout等。

#### 4 layout 过程
>Layout 的 作用是 ViewGroup 用来确定子元素的位置，当 ViewGroup 的位置确定后，它在 onLayout 中会遍历所有子元素并调用其 layout 方法，在 layout 方法中 onLayout 方法又会被调用。

##### 4.1 View 的 layout 过程
View 的 layout 方法定义如下：
`public void layout(int l, int t, int r, int b) `
>layout 方法中会通过 setFrame 方法来设定 View 的四个顶点的位置，即初始化 mLeft，mRight，mTop 和 mBottom，View 的四个顶点一旦确定，那么 View 在父容器中的位置也就确定了。

在 layout 方法中会调用 onLayout 方法，但 View 基类并没有对 onLayout 方法定义具体的实现，onLayout 方法在 View 中的定义如下：
 <center>**View # onLayout(boolean changed, int left, int top, int right, int bottom)**
```java
 protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
    }
```

##### 4.2 ViewGroup 的 layout 过程

 <center>**ViewGroup # layout(int l, int t, int r, int b)**
```java
    @Override
    public final void layout(int l, int t, int r, int b) {
        if (!mSuppressLayout && (mTransition == null || !mTransition.isChangingLayout())) {
            if (mTransition != null) {
                mTransition.layoutChange(this);
            }
            super.layout(l, t, r, b);
        } else {
            // record the fact that we noop'd it; request layout when transition finishes
            mLayoutCalledWhileSuppressed = true;
        }
    }
```
ViewGroup 的 layout 方法 调用了 View 的 layout 方法（`super.layout(l, t, r, b)`），而 View 的 layout 方法内部会调用 onLayout 方法，ViewGroup 覆写了 View 的 onLayout 方法，使 onLayout 方法成为抽象方法，那么 ViewGroup 的子类就必须提供 onLayout 的具体实现。即 ViewGroup 的 layout 依赖于抽象方法 onLayout（细节依赖抽象——依赖倒置原则）。

ViewGroup 和 View 对 onLayout 方法的声明如下：
 <center>**ViewGroup # onLayout(boolean changed, int left, int top, int right, int bottom)**
```java
 @Override
    protected abstract void onLayout(boolean changed,
            int l, int t, int r, int b);
```

ViewGroup 完成自己的 layout 的同时需要遍历所有子元素开启子元素的 layout 过程，但开启子元素的 layout 过程这一点在 ViewGroup 基类中并没有体现，像 ViewGroup 的开启子元素的 measure 过程体现在 ViewGroup 的 measureChild 方法中，在 measureChild 方法中会调用子元素的 measure 方法。这是怎么回事呢？
这就又回到 ViewGroup 的 onLayout 方法上了，既然 ViewGroup 的 onLayout 方法是抽象的，那么开启子元素的 layout 过程就应该在 ViewGroup 的子类的 onLayout 方法中。举个栗子，FrameLayout 的 onLayout 方法：

 <center>**FrameLayout# onLayout(boolean changed, int left, int top, int right, int bottom)**
```java
    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        layoutChildren(left, top, right, bottom, false /* no force left gravity */);
    }
```
看看 layoutChildren 方法。
 <center>**FrameLayout# layoutChildren(int left, int top, int right, int bottom, boolean forceLeftGravity)**
```java
   void layoutChildren(int left, int top, int right, int bottom, boolean forceLeftGravity) {
		...
        for (int i = 0; i < count; i++) {
            final View child = getChildAt(i);
            if (child.getVisibility() != GONE) {
                ...
                switch (verticalGravity) {
                    case Gravity.TOP:
                        childTop = parentTop + lp.topMargin;
                        break;
                    case Gravity.CENTER_VERTICAL:
                        childTop = parentTop + (parentBottom - parentTop - height) / 2 +
                        lp.topMargin - lp.bottomMargin;
                        break;
                    case Gravity.BOTTOM:
                        childTop = parentBottom - height - lp.bottomMargin;
                        break;
                    default:
                        childTop = parentTop + lp.topMargin;
                }

                child.layout(childLeft, childTop, childLeft + width, childTop + height);
            }
        }
    }
```

可以看到在 layoutChildren 方法中遍历了 FrameLayout 的子元素，并调用子元素的 layout 方法。

#### 5 draw 过程
View 的 draw 过程遵循如下步骤：
>1. 绘制背景：background.draw(canvas)
>2. 绘制自己：onDraw
>3. 绘制children：dispatchDraw
>4. 绘制装饰：onDrawScrollBars

该流程在 draw 源码中可以明显看出：
<center>**View # draw(Canvas canvas) **</center>
```java
 @CallSuper
    public void draw(Canvas canvas) {
        final int privateFlags = mPrivateFlags;
        final boolean dirtyOpaque = (privateFlags & PFLAG_DIRTY_MASK) == PFLAG_DIRTY_OPAQUE &&
                (mAttachInfo == null || !mAttachInfo.mIgnoreDirtyState);
        mPrivateFlags = (privateFlags & ~PFLAG_DIRTY_MASK) | PFLAG_DRAWN;

        /*
         * Draw traversal performs several drawing steps which must be executed
         * in the appropriate order:
         *
         *      1. Draw the background
         *      2. If necessary, save the canvas' layers to prepare for fading
         *      3. Draw view's content
         *      4. Draw children
         *      5. If necessary, draw the fading edges and restore layers
         *      6. Draw decorations (scrollbars for instance)
         */

        // Step 1, draw the background, if needed
        int saveCount;

        if (!dirtyOpaque) {
            drawBackground(canvas);
        }

        // skip step 2 & 5 if possible (common case)
        final int viewFlags = mViewFlags;
        boolean horizontalEdges = (viewFlags & FADING_EDGE_HORIZONTAL) != 0;
        boolean verticalEdges = (viewFlags & FADING_EDGE_VERTICAL) != 0;
        if (!verticalEdges && !horizontalEdges) {
            // Step 3, draw the content
            if (!dirtyOpaque) onDraw(canvas);

            // Step 4, draw the children
            dispatchDraw(canvas);

            // Overlay is part of the content and draws beneath Foreground
            if (mOverlay != null && !mOverlay.isEmpty()) {
                mOverlay.getOverlayView().dispatchDraw(canvas);
            }

            // Step 6, draw decorations (foreground, scrollbars)
            onDrawForeground(canvas);

            // we're done...
            return;
        }

       ...
    }
```
draw 方法会依次调用上述流程的对应方法完成自己的绘制和子元素的绘制，这里重点来看看 dispatchDraw 方法，即 draw 流程的分发。

<center>**View # dispatchDraw(Canvas canvas) **</center>
```java
 protected void dispatchDraw(Canvas canvas) {

    }
```

可见 View 并没有对 dispatchDraw 方法提供实现，说明其实现与具体的 View 相关。
##### 5.1 ViewGroup 的 dispatchDraw 方法
ViewGroup 对 dispatchDraw 方法定义了具体的实现，在方法内部调用了 drawChild 方法

<center>**ViewGroup# drawChild(Canvas canvas, View child, long drawingTime) **</center>
```java
protected boolean drawChild(Canvas canvas, View child, long drawingTime) {
        return child.draw(canvas, this, drawingTime);
    }
```
可以看到在 drawChild 方法中会去调用子元素的 draw 方法开启子元素的 draw 流程。

----------
文章大部分内容摘抄自《Android 开发艺术探索》第 4 章 —— View 的工作原理，加上部分自己的理解和总结，可能有错误，欢迎指正。

-----
