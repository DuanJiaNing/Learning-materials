![](http://upload-images.jianshu.io/upload_images/7460499-368d8348d3c69e65.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
#### 圆点指示器支持“纵向视图”啦
关于圆点指示器的实现请参看上一篇博文：[自定义View和属性动画ValueAnimator实现圆点指示器](http://blog.csdn.net/aimeimeits/article/details/69370853)
该篇文章将对支持纵向视图过程的关键代码和使用方法进行介绍。

控件元素的构成
| 小圆点 | 指示点 | 线段 |
| :----- | :--- | :---- |
|圆|椭圆|矩形|

##### 一 效果图
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-605a99d731428008?imageMogr2/auto-orient/strip)

##### 二 如何使用
使用时只需在你的`xml`布局文件中指定`indicatorOrientation`属性为`vertical`即可。
```xml
 <com.duan.indicatorviewdemo.IndicatorView
 
             app:indicatorOrientation="vertical"
             
            android:layout_marginLeft="30dp"
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            app:dotNum="5"
            app:dotSize="20dp"
            app:indicatorSize="20dp"
            app:dotColor="#a5b7b3"
            app:indicatorColor="#52ffda"
            app:lineColor="#cacaca"
            app:lineLength="90dp"
            app:lineWidth="4dp" />
```
##### 三 关键代码
###### 3.1 attr 中添加方向属性定义
本项目在attr.xml文件中添加如下属性定义，并将该属性添加到`declare-styleable`中：
``` xml
  <attr name="indicatorOrientation" format="integer"> <!--控件方向-->
        <enum name="vertical" value="0"/> <!--纵向-->
        <enum name="horizontal" value="1"/> <!--水平-->
    </attr>

  <declare-styleable name="IndicatorView">  
	...
	   <attr name="indicatorOrientation"/>
	...
  </declare-styleable>
```
###### 3.2 修改 IndicatorView.java 
- 添加`mIndicatorOrientation`变量，并在构造函数中获得`xml`布局文件中指定的值，默认为水平方向。
``` java
//默认为 水平
        mIndicatorOrientation = array.getInteger(R.styleable.IndicatorView_indicatorOrientation, INDICATOR_ORIENTATION_HORIZONTAL);
```
- onMeasure 方法
onMeasure 方法需要处理 xml 布局文件设置宽高为`warp_content`的情况，此时需要计算出控件的实际宽和高。
在该方法中判断当设置为“纵向视图”时，宽和高的计算方法为：
宽 = 左内边距 + 右内边距 + 指示点大小
高 = 上内边距 + 下内边距 + 线段总长 + 指示点大小
```
...
if (widthMode == MeasureSpec.EXACTLY) {
            width = widthSize;
        } else {//xml中宽度设为warp_content
            if (mIndicatorOrientation == INDICATOR_ORIENTATION_VERTICAL) //纵向
                width = getPaddingLeft() + getPaddingRight() + mIndicatorSize;
            else
                width = getPaddingLeft() + ((mDotCount - 1) * mLineLength + mIndicatorSize) + getPaddingRight();
        }

        if (heightMode == MeasureSpec.EXACTLY) {
            height = heightSize;
        } else {
            if (mIndicatorOrientation == INDICATOR_ORIENTATION_VERTICAL) //纵向
                height = ((mDotCount - 1) * mLineLength + mIndicatorSize) + getPaddingBottom() + getPaddingTop();
            else
                height = getPaddingTop() + mIndicatorSize + getPaddingBottom();
        }
        ...
```
- onLayout 方法
在 onLayout 方法中会初始化存储指示点属性值的类，在这里需要对不同的控件方向做不同的处理。
控件为“纵向视图”时，指示点（椭圆）的中心的横坐标是固定的，中心的纵坐标与当前指示点所在的位置有关。而控件为“水平视图”时纵坐标固定，横坐标与所在位置有关
```
 if (mIndicatorOrientation == INDICATOR_ORIENTATION_VERTICAL) { //纵向
                indicatorHolder.setCenterX(getWidth() / 2);
                indicatorHolder.setCenterY(mIndicatorPos * mLineLength + getPaddingBottom() + mIndicatorSize / 2);
            } else {
                indicatorHolder.setCenterX(mIndicatorPos * mLineLength + getPaddingLeft() + mIndicatorSize / 2);
                indicatorHolder.setCenterY(getHeight() / 2);
            }
```
- onDraw 方法绘制线段
 线段使用的是`Canvas`的`drawRect(float left, float top, float right, float bottom, @NonNull Paint paint) `方法绘制的。
 画线段时需要对控件方向做出判断，控件为“纵向视图”时，线段（矩形）的 left 和 right 是固定的，而 top 和 bottom 与当前正在绘制的线段有关。“水平视图”时 top 和 bottom 固定，right 和 left 与当前正在绘制的线段有关。
```java
     ...
        if (mIndicatorOrientation == INDICATOR_ORIENTATION_VERTICAL) { //纵向
            for (int i = 0; i < mDotCount - 1; i++) {
                int top = getHeight() - (getPaddingBottom() + mIndicatorSize / 2 + mLineLength * (i + 1));
                int bottom = getHeight() - (getPaddingBottom() + mIndicatorSize / 2 + mLineLength * i);
                int left = (getWidth() - mLineWidth) / 2;
                int right = (getWidth() + mLineWidth) / 2;

                canvas.drawRect(left, top, right, bottom, mPaint);
                }
    ...
```

 - onDraw 方法绘制小圆点
 小圆点使用的是`Canvas`的`drawCircle(float cx, float cy, float radius, @NonNull Paint paint)`方法，当控件为“纵向视图”时，cx 值是固定的，cy 需要计算。
```java
...
if (mIndicatorOrientation == INDICATOR_ORIENTATION_VERTICAL) { //纵向
            for (int i = 0; i < clickableAreas.length; i++) {
                int cx = getWidth() / 2;
                int cy = i * mLineLength + getPaddingBottom() + mIndicatorSize / 2;

                if (switchTo != -1 && i == switchTo)
                    mPaint.setColor(mIndicatorColor);
                else
                    mPaint.setColor(mDotColor);

                canvas.drawCircle(cx, cy, mDotSize, mPaint);
                clickableAreas[i][0] = cx;
                clickableAreas[i][1] = cy;
            }
            ...
```
-  onDraw 方法绘制指示点
   指示点的绘制时根据`indicatorHolder`对象中存的属性值进行绘制的(getter方法)，所有无需修改，需要修改的是对`indicatorHolder`对象值的控制(setter操作)，即平移动画的定义和“挤压”动画的定义。

- onTouchEvent 方法
onTouchEvent 监听对控件的触摸事件，这个方法里需要修改连个地方
1 判断当前手指所在的小圆点：为“水平视图”时是根据`event.getX()`的值判断所在小圆点的位置，为“纵向视图”时应该根据`event.getY()`来判断。
2 当用户“拖拽”指示点时：为“水平视图”时这里要修改`indicatorHolder`的中心的横坐标，为“纵向视图”则应该修改其中心的纵坐标。
``` java
...
 int ey = (int) event.getY();
 ...
 //纵向
   for (; switchTo < mDotCount; switchTo++) {
                int[] xy = clickableAreas[switchTo];
                //只对y坐标位置进行判断，这样即使用户手指在控件外面（先在控件内触摸后不抬起而是滑到控件外面）滑动也能判断
                if (ey <= xy[1] + temp && ey >= xy[1] - temp) {
                    break;
                }
            }
...
if (mIndicatorOrientation == INDICATOR_ORIENTATION_VERTICAL) { //纵向
                    indicatorHolder.setCenterY(ey);
                } 
                ...
```
- 指示点切换动画
平移动画：为“纵向视图”时平移动画应在`indicatorHolder`中心的纵坐标上做变化。
```java
...
 if (mIndicatorOrientation == INDICATOR_ORIENTATION_VERTICAL) { //纵向
            int start = indicatorHolder.getCenterY();
            end = switchTo * mLineLength + getPaddingBottom() + mIndicatorSize / 2;
            trainsAnim = ObjectAnimator.ofInt(indicatorHolder, "centerY", start, end);
        }
...
```
挤压动画：为“纵向视图”时挤压动画改变`indicatorHolder`的宽度（centerW）因为线段的宽度，高度（centerH）为指示点当前所在位置和目标点间的距离。
```java
...
if (mIndicatorOrientation == INDICATOR_ORIENTATION_VERTICAL) { //纵向
                        centerH = Math.abs(indicatorHolder.getCenterY() - clickableAreas[switchTo][1]);
                        centerW = mLineWidth;
                    } 
...
```
不使用动画：此时判断为“纵向视图”时应修改`indicatorHolder`的中心的纵坐标。
```java
...
if (mIndicatorOrientation == INDICATOR_ORIENTATION_VERTICAL) //纵向
                        indicatorHolder.setCenterY(end);
...
```

<center>代码修改已同步到Github，你可以在这里下载到源代码：</center><br><center>[DuanJiaNing/IndicatorView](https://github.com/DuanJiaNing/IndicatorView)</center>

----------
