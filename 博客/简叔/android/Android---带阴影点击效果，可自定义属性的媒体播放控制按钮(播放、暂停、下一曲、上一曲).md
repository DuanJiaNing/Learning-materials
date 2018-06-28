![](http://upload-images.jianshu.io/upload_images/7460499-ee12a391d71c6deb.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
#### Android - 带阴影点击效果，可自定义属性的媒体播放控制按钮(播放、暂停、下一曲、上一曲)

>自定义 View 开发的可定制大小，颜色，圆角，阴影半径，描边风格，点击时有”荧光“效果的播放(暂停)，上一曲(下一曲)按钮。

demo 已上传 GitHub ，欢迎下载使用，有问题可以给我留言。
仓库地址：
[DuanJiaNing/MediaView](https://github.com/DuanJiaNing/MediaView)

#### 一.开发背景
网易云音乐的很多按钮控件点击时都是带阴影效果的，不同于 material design 的涟漪效果，按钮被点击时图标轮廓四周会有白色“阴影”， 或者可以称为“荧光”效果，直接使用图标虽然可以达到同样的效果，但毕竟不灵活，可定制属性较少，于是，不如自己写一个。

#### 二.效果图
![](http://upload-images.jianshu.io/upload_images/7460499-b79e1300e82dea24.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![](https://raw.githubusercontent.com/DuanJiaNing/MediaView/master/screenshort.gif)
#### 三.介绍
> 控件具有如下继承结构
> |-- android.view.View
>      |-- abstract MediaView
>            |-- SkipView    上一曲(下一曲)按钮
>                 |-- PlayView     播放按钮
##### 3.1 构成元素

xml 中提供的可定制属性如下：

- radius 半径：整个控件的半径
- shadowRadius 阴影半径

（1） 外圆圈

- strokeWidth 描边宽度
- strokeColor 描边颜色


（2） 上一曲(下一曲)按钮：

- distance 单竖线和三角形顶点距离

1 单竖线：

- innerLineWidth(Height) 宽度，高度：任一者赋值为 0 时不进行绘制。
- innerLineRadius 圆角大小

2 等腰三角形：

- triangleWidth 三角形顶角到底边的距离
- triangleHeight 底边高度
- triangleColor 填充颜色
- triangleRadius 圆角大小
- triangleHollow 是否空心
- triangleStroke 空心时的描边宽度

（3） 播放，暂停按钮

- checked 是否播放，true 为正在播放（此时处于可暂停状态）

1 播放状态：
播放状态下直接绘制继承自 SkipView 的等腰三角形

2 暂停状态：此时两条竖线的属性时完全一致的

- pauseLineDistance 双竖线间距
- pauseLineWidth 竖线宽度
- pauseLineHeight 竖线高度
- pauseLineRadius 竖线圆角
- pauseLineColor 竖线颜色
- pauseLineHollow 是否空心
- pauseLineStroke 空心状态下指定竖线描边宽度

![](http://upload-images.jianshu.io/upload_images/7460499-e21c6305f34735ce.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### 四. 具体实现

##### 4.1 MediaView
该类的完整定义如下：
`public abstract class MediaView extends View implements ValueAnimator.AnimatorUpdateListener`
将媒体控件共有的特性提取出来写在这个类中，共有特性有：
1 控件中心所在位置。
2 控件中心作为圆心，向外延展的半径。
3 圆圈的描边宽度
4 触摸动画的执行：有两个动画，手指触摸到控件时阴影由无到有的动画，手指离开屏幕时阴影从有到无。

###### onMeasure 方法
在该方法中计算出控件的宽高，以及半径。
注意：当 xml 中指定控件的宽高为具体值（math_parent 或 具体值）时控件的半径依照如下规则计算，此时在 xml 中指定的半径将失效：

控件宽高分别减去上下 padding 和左右 padding 后值小的一方减去阴影半径和圆圈描边之和的两倍除以 2（哈哈 还不如直接看代码！）。
对应代码为：` radius = (Math.min(width - wp, height - hp) - t) / 2;`

当宽高都为 warp_content 时，xml 中指定的半径有效，此时控件高宽计算方式如下：
` width = radius * 2 + t + wp;`
`height = radius * 2 + t + hp;`
此时要考虑的因素有直径，阴影直径，两侧描边之后以及上下 padding(左右 padding)。
以高为例：高为半径的 2 倍加上阴影半径和描边宽度和的两倍加上上下侧的 padding 。
```java

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int widthMode = MeasureSpec.getMode(widthMeasureSpec);
        int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightMode = MeasureSpec.getMode(heightMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);
        int width;
        int height;

        int t = (shadowRadius + strokeWidth) * 2;
        int wp = getPaddingLeft() + getPaddingRight();
        int hp = getPaddingTop() + getPaddingBottom();

        int te = -1;
        if (widthMode == MeasureSpec.EXACTLY) {
            width = widthSize;
            te = 0;
        } else {//xml中宽度设为warp_content
            width = radius * 2 + t + wp;
        }

        if (heightMode == MeasureSpec.EXACTLY) {
            height = heightSize;
            te = 0;
        } else {
            height = radius * 2 + t + hp;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            width = Math.max(width, getMinimumWidth());
            height = Math.max(height, getMinimumHeight());
        }

        setMeasuredDimension(width, height);

        //长宽任一者指定具体长度（EXACTLY）时，xml 中设置的 radius 失效
        if (te != -1)
            radius = (Math.min(width - wp, height - hp) - t) / 2;

    }
```
###### onLayout 方法
在 onLayout 方法中确定圆心，注意 getWidth getHeight 只有在 onMeasure 方法执行完之后才能正确获得值，确定规则如下：
```java
 centerX = getPaddingLeft() + (getWidth() - getPaddingLeft() - getPaddingRight()) / 2;
  centerY = getPaddingTop() + (getHeight() - getPaddingTop() - getPaddingBottom()) / 2;
```
###### onTouchEvent 方法
覆写该方法控制触摸动画的执行，点击事件监听。
注意：当**收到 ACTION_DOWN 事件之后，先调用父类的 onTouchEvent 方法，然后再返回 true 。**

这样做的原因是：在收到 ACTION_DOWN 事件后启动触摸动画（阴影从无到有），在 ACTION_UP 事件到达时恢复（启动释放动画，阴影从有到无），既然需要 ACTION_UP 事件，意味着必须处理  ACTION_DOWN  事件（收到 ACTION_DOWN  时返回 true ，如果返回 false ，那后续的事件就不会传到该控件，也就收不到 ACTION_UP 事件）。

事件分发机制可参看我的另一篇文章：[Android-View的事件分发机制](http://blog.csdn.net/aimeimeits/article/details/71512627)
但这样有个问题，返回 true 意味着我（该控件）对系统宣布：此次事件序列接下来发生的所有事件都交给我处理，那点击事件呢？长按事件呢？返回 true 后系统也就不再帮你决定何时触发点击事件了，即点击事件的触发也由你自己决定，但我不想自己控制点击事件的触发时机（有些麻烦），该怎么办呢？

ACTION_DOWN 事件返回 true 前的`super.onTouchEvent(event);`就可以解决这个问题，在ACTION_UP 到达时也要调用`super.onTouchEvent(event);`，这样系统仍将帮你处理点击事件。即在外面为控件绑定点击事件监听时能正常回调。
原因：
View 的点击事件在 ACTION_DOWN 到达时开始，系统会启动一个计时器，长按到一定时间后系统会触发 onLongClick 事件，快速单击时直接触发 onClick 事件，这个过程是在 ACTION_DOWN  时开始，在 ACTION_UP 时决策是否触发点击事件，在 ACTION_DOWN   和  ACTION_UP 之间决策是否触发长按事件。因此在 ACTION_DOWN    和 ACTION_UP 到达时到调用 View 的 onTouchEvent 方法以完成点击，长按事件触发控制。
```java
 @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                startPreAnim();
                //调用 View 的事件监听以使用 View 的 click 和 longClick 监听
                super.onTouchEvent(event);
                return true;
            case MotionEvent.ACTION_UP:
                startReleaseAnim();
                //调用 View 的事件监听以使用 View 的 click 和 longClick 监听
                super.onTouchEvent(event);
                break;
            default:
                break;
        }
        return false;
    }
```
###### onDraw 方法
该方法以 final 的形式被覆写，这意味着任何继承于 MediaView 的控件都无法覆盖该方法，这主要是为了限制 MediaView  子类控件的UI风格，如果子类重写了 onDraw 方法，那何不直接继承 View，定义一个全新的控件。

`isCreate`变量用于标识是否为第一次绘制，控件阴影初始时并不是 0 ，如果直接绘制，会把阴影也绘制出来，所以第一次绘制时不绘制阴影，之后的重绘（主要是动画时）由动画控制阴影的大小。
在调用`setShadowRadius`方法设置阴影大小时，修改阴影的同时将更新属性动画的参数。

` paint.setMaskFilter(new BlurMaskFilter(sd, BlurMaskFilter.Blur.SOLID));`
设置画笔绘制图形内容的同时绘制外阴影，不绘制内阴影。

通过 Paint 的`setMaskFilter`方法设置阴影大小，注意 `BlurMaskFilter`的构造方法第一个参数是浮点类型，且值必须为大于 0 ，因而无论是属性动画中或是 onDraw 方法中的局部变量`sd`，亦或`setShadowRadius`方法，阴影的最小值不会小于 1。

setMaskFilter  方法在开启硬件加速的情况下是无效的，因而要关闭硬件加速。
```java
//Android4.0（API14）之后硬件加速功能就被默认开启了,setMaskFilter 在开启硬件加速的情况下无效，需要关闭硬件加速
this.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
```
Paint 的`setMaskFilter`具体介绍请参考这里：[详解Paint的setMaskFilter(MaskFilter maskfilter)](http://blog.csdn.net/nimeghbia/article/details/48891557)

先绘制圆圈，在绘制内部。drawInside 方法是抽象方法，由子类实现。
`    protected abstract void drawInside(Canvas canvas);`
```java
    @Override
    protected final void onDraw(Canvas canvas) {

        canvas.drawARGB(0, 0, 0, 0);

        int sd;
        if (isCreate) {
            //第一次绘制时使阴影为0
            //之后重绘时由动画控制阴影的大小
            sd = 1;
            isCreate = false;
        } else sd = shadowRadius;

        //只绘制外阴影和图形内容本身，不绘制内阴影
        paint.setMaskFilter(new BlurMaskFilter(sd, BlurMaskFilter.Blur.SOLID));

        drawOuter(canvas);

        drawInside(canvas);

    }
```


##### 4.2 SkipView
类定义：`public class SkipView extends MediaView`
该类完成【上一曲(下一曲)】控件的绘制。
该类继承了 MediaView 的属性和方法的同时又很多自己的属性，可以参看上面的 <a href="#">3.1 构成元素 
（2） 上一曲(下一曲)按钮</a> 查看具体属性

###### onLayout 方法

![](http://upload-images.jianshu.io/upload_images/7460499-e21c6305f34735ce.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

1 在 onLayout 方法中检查【单竖线】的高宽是否小于等于 0，如果任一者赋值为 0，就将两者赋为 0，这样在绘制时就不会绘制单竖线了。

2 如果三角形的高（底边的长度）小于或等于 0 （这可能是因为 xml 中赋值错误，或没有赋值（SkipView 的构造方法中将底边的长度默认值设为 0），就将其赋为半径的 2/3。

**下面提到的 顶点 指的都是等腰三角形 顶角 所在的顶点。**

3 如果【三角形顶点与底边的距离】小于或等于 0 ，就将该等腰三角形作为等边三角形，计算出值。

4 三角形圆角的绘制使用的是 Path 的 cubicTo 方法，该方法需要指定三个点作为控制点绘制贝塞尔曲线。
据此计算出三角形上 9 个点的坐标，9 个点的坐标在图中第三个控件中有标明。计算时根据对称性可简化一些计算。

5 *很关键的计算过程建议直接看代码，参照注释理解。*
因为计算的时候是假设【三角形顶点与底边连线】的中点与圆心重合进行计算的，所以 9 个点计算出来后需要进行平移，使三角形的【内心】与圆心重合。这里需要动手推一下平移距离计算公式，即让两点重合需要平移的距离。

* 使两点重合平移（往前移）的距离：先计算出内心与顶点的距离，该距离减去【三角形顶角与底边连线】的一半。
* 两点重合之后还需要移动（往后移）【单竖线宽度】+【竖线与顶点间距】和的一半。原因是需要把【竖线+间距+三角形】整体移动到【圆圈】中心，这样内部整体才居中。

移动时只需修改三角形 9 个顶点的横坐标即可，竖线的位置根据三角形顶点来确定，所以不用考虑。 
```java
 @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);

        if (innerLineHeight <= 0 || innerLineWidth <= 0) {
            innerLineHeight = 0;
            innerLineWidth = 0;
        }

        if (triangleHeight <= 0)
            triangleHeight = radius * 2 / 3;

        if (triangleWidth <= 0) {
            //将三角形绘制成等边三角形
            int t2 = triangleHeight * triangleHeight;
            triangleWidth = (int) Math.sqrt(t2 - (t2 / 4));
        }

        //赋值不合理，进行重置（这不是此控件预期的展现形式，应根据整体比例合理赋值）
        //只对上限进行处理
        if (innerLineHeight >= radius * 2)
            innerLineHeight = radius * 2;
        if (innerLineWidth >= radius)
            innerLineWidth = radius;
        if (triangleWidth >= radius)
            triangleWidth = radius;
        if (triangleHeight > radius * 2)
            triangleHeight = radius * 2;

        //以顶点作为参考
        //圆角导致的纵坐标迁移量
        float ry = triangleRadius / 2;
        //圆角导致的横坐标迁移量
        float rx = (float) Math.sqrt(triangleRadius * triangleRadius - ry * ry);

        //计算三角形上 9 个点的坐标
        //拟使【三角形顶点与底边中点相连的线段的中点】在【圆心】上进行计算
        //计算时根据三角形在坐标轴上的位置关系简化计算
        int halfW = triangleWidth / 2;
        coordinate[0][0] = centerX - halfW;
        coordinate[0][1] = centerY;
        coordinate[1][0] = (centerX - halfW) + rx;
        coordinate[1][1] = centerY - ry;
        coordinate[8][0] = coordinate[1][0];
        coordinate[8][1] = centerY + ry;

        coordinate[3][0] = centerX + halfW;
        coordinate[3][1] = centerY - triangleHeight / 2;
        coordinate[2][0] = (centerX + halfW) - rx;
        coordinate[2][1] = centerY - (triangleHeight / 2 - ry);
        coordinate[7][0] = coordinate[2][0];
        coordinate[7][1] = centerY + (triangleHeight / 2 - ry);

        coordinate[6][0] = coordinate[3][0];
        coordinate[6][1] = centerY + triangleHeight / 2;
        coordinate[4][0] = centerX + halfW;
        coordinate[4][1] = centerY - (triangleHeight / 2 - triangleRadius);
        coordinate[5][0] = coordinate[4][0];
        coordinate[5][1] = centerY + (triangleHeight / 2 - triangleRadius);

        //平移三角形使其居中
        //平移之前【三角形顶点与底边中点相连的线段中点】与【圆心】重合，此时内部的【整体图形】没有在圆圈内【居中】
        //平移所有点使【三角形内心】与【圆心】重合
        // ** 计算【等边三角形内心与顶点的距离】
        double a2 = triangleWidth * triangleWidth;
        double c2 = triangleHeight * triangleHeight / 4;
        double c = triangleHeight / 2;
        double a = triangleWidth;
        double x = (Math.pow(Math.sqrt(a2 + c2) - c, 2) + a2) / (2 * a);

        //往前移动【三角形内心到圆心位置】，然后往后移动【竖线宽度】和【顶点到竖线距离】之和的一半
        double tr = (x - halfW) - (distance + innerLineWidth) / 2;
        for (int i = 0; i < coordinate.length; i++) {
            coordinate[i][0] -= tr;
        }

    }
```
###### drawInside 方法
覆写 MediaView 的 drawInside 方法
```java
    @Override
    public void drawInside(Canvas canvas) {
        drawLine(canvas);
        drawTriangle(canvas);
    }
```
###### drawLine 和 drawTriangle 方法

```java
    /**
     * 绘制单竖线
     */
    protected void drawLine(Canvas canvas) {

        if (innerLineHeight <= 0 || innerLineWidth <= 0)
            return;

        paint.setStyle(Paint.Style.FILL);
        paint.setColor(triangleColor);

        //绘制竖线
        //参考三角形的 顶点 绘制
        float left = coordinate[0][0] - distance - innerLineWidth;
        float top = coordinate[0][1] - innerLineHeight / 2;
        float right = left + innerLineWidth;
        float bottom = top + innerLineHeight;
        RectF rectF = new RectF(left, top, right, bottom);
        canvas.drawRoundRect(rectF, innerLineRadius, innerLineRadius, paint);

    }

    /**
     * 绘制三角形
     */
    protected void drawTriangle(Canvas canvas) {

        if (triangleHollow) {
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeWidth(triangleStroke);
        } else
            paint.setStyle(Paint.Style.FILL);

        paint.setColor(triangleColor);

        //绘制三角形
        //顶点上的点及其两侧的点使用贝塞尔曲线连接，实现圆弧效果
        Path path = new Path();
        path.moveTo(coordinate[1][0], coordinate[1][1]);
        path.lineTo(coordinate[2][0], coordinate[2][1]);
        path.cubicTo(coordinate[2][0], coordinate[2][1], coordinate[3][0], coordinate[3][1], coordinate[4][0], coordinate[4][1]);
        path.lineTo(coordinate[5][0], coordinate[5][1]);
        path.cubicTo(coordinate[5][0], coordinate[5][1], coordinate[6][0], coordinate[6][1], coordinate[7][0], coordinate[7][1]);
        path.lineTo(coordinate[8][0], coordinate[8][1]);
        path.cubicTo(coordinate[8][0], coordinate[8][1], coordinate[0][0], coordinate[0][1], coordinate[1][0], coordinate[1][1]);
        path.close();

        canvas.drawPath(path, paint);
    }
```

##### 4.3 PlayView
【播放(暂停)】按钮
类定义：`public class PlayView extends SkipView implements Checkable `
该类继承了 SkipView 的属性和方法的同时又很多自己的属性，可以参看上面的 <a href="#">3.1 构成元素 
（3） 播放，暂停按钮</a> 查看具体属性

###### drawInside 方法
覆写了 SkipView 的 drawInside 方法
在【正在播放】状态下绘制双竖线，`drawLine` 方法也覆写了 SkipView 的方法。
在【没有播放】状态下直接调用父类的 `drawTriangle`方法绘制三角形。
```java
    @Override
    public void drawInside(Canvas canvas) {
        if (isPlaying) {
            drawLine(canvas);
        } else {
            super.drawTriangle(canvas);
        }
    }
```
###### onTouchEvent 方法
覆写了父类的方法，但只修改了少部分，在 ACTION_UP 事件到达时反转当前状态。
```java
 @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                startPreAnim();
                //调用 View 的事件监听以使用 View 的 click 和 longClick 监听
                super.onTouchEvent(event);
                return true;
            case MotionEvent.ACTION_UP:
                startReleaseAnim();
                //状态反转
                toggle();
                //调用 View 的事件监听以使用 View 的 click 和 longClick 监听
                super.onTouchEvent(event);
                break;
            default:
                break;
        }

        return false;
    }
```
###### toggle 方法
该方法覆写自 `android.widget.Checkable`接口，反转当前选中状态。
```java
 //反转状态
    @Override
    public void toggle() {
        isPlaying = !isPlaying;
        if (checkedChangeListener != null)
            checkedChangeListener.onCheckedChanged(this, isPlaying);
        invalidate();
    }
```

###### drawLine 方法
该方法中绘制 【播放状态】下的双竖线，绘制时需要以圆点为中心，在两侧绘制两条竖线，这样就能使两条竖线居中。
```java

    @Override
    protected void drawLine(Canvas canvas) {

        if (pauseLineHollow) {
            paint.setStyle(Paint.Style.STROKE);
            paint.setStrokeWidth(pauseLineStroke);
        } else
            paint.setStyle(Paint.Style.FILL);

        paint.setColor(pauseLineColor);

        //绘制双竖线
        float left = centerX - pauseLineDistance / 2 - pauseLineWidth;
        float top = centerY - pauseLineHeight / 2;
        float right = left + pauseLineWidth;
        float bottom = top + pauseLineHeight;
        RectF rectF = new RectF(left, top, right, bottom);
        canvas.drawRoundRect(rectF, pauseLineRadius, pauseLineRadius, paint);

        float l = right + pauseLineDistance;
        float t = top;
        float r = l + pauseLineWidth;
        float b = bottom;
        RectF rf = new RectF(l, t, r, b);
        canvas.drawRoundRect(rf, pauseLineRadius, pauseLineRadius, paint);
    }

```
#### 五.如何使用
##### 5.1 复制源文件
使用 【上一曲(下一曲)】和【播放，暂停】控件需复制 MediaView.java ， SkipView.java ， PlayView.java 和 attrs.xml 文件到你的项目中。

如果你只需要 【上一曲(下一曲)】对应的控件：
1 复制 MediaView.java 和 SkipView.java 到你的项目中
2 将 attrs 文件中的 ` <declare-styleable name="MediaView">....</declare-styleable>`及其对应的 attr 属性定义，` <declare-styleable name="SkipView">....</declare-styleable>`及其对应的 attr 属性定义 复制到你项目中的 values 文件夹下的资源文件中。

##### 5.2 使用示例
在完成 5.1 之后，需要 Rebuild Project ，然后就可以在你的项目中使用了。

可以在布局文件中直接使用：
使用时包名替换成你的源文件所在位置

**【上一曲(下一曲)】控件**
```xml
        <com.duan.mediaviewdemo.view.SkipView
            android:id="@+id/sv_01"
            android:layout_width="0dp"
            android:layout_height="match_parent"
            android:layout_weight="0.5"

            app:innerLineHeight="30dp"
            app:innerLineWidth="6dp"

            app:innerLineRadius="8dp"

            app:distance="-8dp"
            app:triangleHeight="40dp"

            app:strokeWidth="0dp"
            app:triangleColor="#2ca4a4"
            app:triangleRadius="8dp"

            />
```

**【播放，暂停】控件**
``` xml
      <com.duan.mediaviewdemo.view.PlayView
            android:layout_width="0dp"

            android:layout_height="match_parent"
            android:layout_weight="0.5"

            android:rotation="180"
            app:checked="true"
            app:pauseLineDistance="8dp"
            app:pauseLineHeight="35dp"

            app:pauseLineHollow="true"

            app:pauseLineRadius="0dp"
            app:pauseLineWidth="10dp"
            app:strokeColor="#e4188f"
            app:triangleColor="#e4188f"
            app:strokeWidth="3dp"
            app:triangleHeight="35dp"


            app:triangleHollow="true"
            app:triangleRadius="5dp"
            app:triangleStroke="7dp" />
```
在 java 中使用：
```java
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SkipView sv = (SkipView) findViewById(R.id.sv_01);
        PlayView pv = (PlayView) findViewById(R.id.pv_01);

        sv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(MainActivity.this, "click", Toast.LENGTH_SHORT).show();
            }
        });

        pv.setOnCheckedChangeListener(new PlayView.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(PlayView view, boolean checked) {
                Toast.makeText(MainActivity.this, "checked:" + checked, Toast.LENGTH_SHORT).show();
            }
        });

    }
```
-----------
代码有点多，说的也可能不清楚  (0.0)，不如直接看代码吧。demo 已上传 GitHub ，欢迎下载使用，有问题可以给我留言。
仓库地址：[DuanJiaNing/MediaView](https://github.com/DuanJiaNing/MediaView)
