![](http://upload-images.jianshu.io/upload_images/7460499-f437d24843677b52.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 自定义View和属性动画ValueAnimator实现圆点指示器

>**自定义View和属性动画相结合实现支持动态修改指示点位置，拖拽或点击改变指示点位置，点击位置监听及切换动画自定义的圆点指示器。**

####效果图
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-fd12b608ec252bca?imageMogr2/auto-orient/strip)
<font size="2" color="#a8a8a8">最下面那个“吸干”，想不出用什么词形容更好>.<，后来改回"挤扁"</font>

自定 View 代码写在 `IndicatorView.java`中
#### IndicatorView由以下几个重要的图形构成
- 小圆点：固定不动的圆形
- 指示点：在小圆点上来回移动，通过改变指示点当前所在位置来实现 `指示器` 的功能，为了实现“挤扁”的动画效果，绘制时用的是椭圆。
- 线段：用于连接两个小圆点，绘制时以两个相邻小圆点间的距离为一个 `线段` 单位。循环绘制 `线段` ，绘制`小圆点个数减一` 次后连通所有小圆点，*在布局文件或代码中可修改其可见性（`lineVisible`）*
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-f7ae3c0c765b48b6?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
#### 实现的功能：
- 支持通过xml定义IndicatorView的属性
 - 属性包括：
1 指示点大小、颜色
2 固定显示的小圆点的大小、颜色以及数量
3 连接小圆点的线条的可见性，线条宽度、长度、颜色
4 默认提供了两个用于指示点间切换的动画（平移和挤扁），也可选择不使用动画或自定义
5 默认提供的切换动画的时间可指定
6 启用/禁用拖拽切换（点击切换或两者）功能
- 通过代码动态修改部分属性
- 通过代码或得属性值，如当前指示点位置，颜色等
- 通过代码自定义指示点间切换动画，指示点被触摸的反馈动画及点击事件监听的回调

#### 属性定义在`src/main/res/values/attrs.xml`文件中。
``` xml
<?xml version="1.0" encoding="utf-8"?>
<resources>

    <attr name="lineColor" format="color" /> <!--线条颜色 -->
    <attr name="lineVisible" format="boolean" /> <!--线条是否可见 -->
    <attr name="lineWidth" format="dimension" /> <!--线条长度 -->
    <attr name="lineHeight" format="dimension" /> <!--线条高度 -->

    <attr name="dotSize" format="dimension" /> <!--小圆点大小 -->
    <attr name="dotNum" format="integer"/> <!--小圆点个数 -->
    <attr name="dotColor" format="color" /> <!--小圆点颜色 -->

    <attr name="indicatorColor" format="color" /> <!--指示点颜色 -->
    <attr name="indicatorSize" format="dimension" /> <!--指示点大小 -->
    <attr name="indicatorPos" format="integer"/> <!--指示点位置 -->

    <attr name="duration" format="integer"/> <!--两点间移动动画时间 -->

    <attr name="dotClickEnable" format="boolean"/><!--小圆点点点击时是否把指示点移到点击的小圆点处，若置为false则只能通过setIndicatorPos()方法改变指示点位置-->
    <attr name="indicatorDragEnable" format="boolean"/><!--指示点拖拽是否可用-->
    <attr name="touchEnable" format="boolean"/><!--同时禁用小圆点点击和指示点拖拽-->

    <attr name="IndicatorSwitchAnimation" format="integer"> <!--使用默认提供的动画-->
        <enum name="translation" value="1"/> <!--平移-->
        <enum name="squeeze" value="2"/><!--"挤扁"-->
        <enum name="none" value="0"/> <!--不使用动画-->
    </attr>

    <declare-styleable name="IndicatorView">
        <attr name="dotColor" />
        <attr name="dotSize" />
        <attr name="dotNum" />
        <attr name="lineColor" />
        <attr name="lineVisible" />
        <attr name="lineWidth" />
        <attr name="lineHeight" />
        <attr name="indicatorColor" />
        <attr name="indicatorSize" />
        <attr name="indicatorPos" />
        <attr name="duration" />
        <attr name="IndicatorSwitchAnimation"/>
        <attr name="dotClickEnable"/>
        <attr name="indicatorDragEnable"/>
        <attr name="touchEnable"/>
    </declare-styleable>

</resources>
```
可以在布局文件中直接使用：

``` xml
	  <com.duan.indicatorviewdemo.IndicatorView
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"

        app:IndicatorSwitchAnimation="squeeze"
        app:dotColor="#2d2b2b"
        app:dotNum="4"
        app:dotSize="10dp"
        app:duration="800"
        app:indicatorColor="#ff9500"
        app:indicatorPos="1"

        app:indicatorSize="25dp"
        app:lineColor="#b3b3b3"
        app:lineHeight="4dp"
        app:lineWidth="85dp" />
```

#### 来看下实现代码吧（IndicatorView.java）
##### 几个重要的变量的作用要先了解一下：
``` java
	...........
	/**
     * 保存所有小圆点的圆点坐标，用于在touch事件中判断触摸了哪个点
     */
    private int[][] clickableAreas;

    /**
     * 指示点，不断修改它的属性从而实现动画（属性动画）
     */
    private IndicatorHolder indicatorHolder;

    /**
     * 指示点要移动到的目标位置
     */
    private int switchTo = -1;

    /**
     * 手松开后根据该变量判断是否需要启动切换动画
     */
    private boolean haveIndicatorAniming = false;

    /**
     * 指示点是否被拖拽过，当指示点被拖拽了但没有超过当前指示点位置范围时使之回到原位
     */
    private boolean haveIndicatorDraged = false;

    /**
     * 保存转移动画开始时线的颜色
     */
    private int tempLineColor;
    .........
```
#####  onMeasure方法
- 在 onMeasure 方法中要把自定义 view 的宽和高计算出来。
- 如果在 xml 中指定具体的宽高值或为 match_parent 时不做计算，因为此时宽高就是指定的值或是填满父布局后的宽高。
- 当 xml 中宽高设为 warp_content 时需要进行计算
 -  计算宽度的规则：左右的内边距 + 所有 `线段` 加起来的总长度
 -  计算高度的规则：上下内边距 + `指示点` 的高度。注意：使用默认的 `指示点` 触摸反馈动画时要加上高度差
-  `setPadding(getPaddingLeft() + mIndicatorSize / 3......`这一句是为了在使用默认的 `指示点` 触摸反馈动画，或是自定义动画中有使指示点放大的情况下要多留些空间给控件，否则 `指示点`放大后超出控件高度的部分就不会被绘制（不会显示）。
``` java
@Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int widthMode = MeasureSpec.getMode(widthMeasureSpec);
        int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightMode = MeasureSpec.getMode(heightMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);
        int width;
        int height;
        setPadding(getPaddingLeft() + mIndicatorSize / 3,getPaddingTop(),getPaddingRight() + mIndicatorSize / 3,getPaddingBottom());
        if (widthMode == MeasureSpec.EXACTLY) {
            width = widthSize;
        } else {
            //xml中宽度设为warp_content
            width = getPaddingLeft() + ((mDotCount - 1) * mLineWidth + mIndicatorSize) + getPaddingRight();
        }

        if (heightMode == MeasureSpec.EXACTLY) {
            height = heightSize;
        } else {
            height = getPaddingTop() + mIndicatorSize + getPaddingBottom();
        }

        //若使用默认的指示点触摸动画（放大+渐变颜色）需要加上放大后指示点与放大前指示点的高度差
        //使用自定义时动画时则不加
        setMeasuredDimension(width, mPressAnimator == null ? height + mIndicatorSize / 2 : height);

    }
```
#####  onLayout方法

在onLayout方法中要对 `indicatorHolder`变量进行初始化。
######关于 `indicatorHolder`变量：` private IndicatorHolder indicatorHolder;`
- `IndicatorHolder` 类的实例 `indicatorHolder` 即为属性对象的`target`，控件默认定义好了三个属性动画：
 - 颜色渐变+缩放：`指示点` 的触摸反馈
 - 平移+压扁+拉伸：`指示点` 切换
 - 平移： `指示点` 切换
- 自定义动画也限定只能将`indicatorHolder`作为属性动画的target来实现动画

>上面三个动画都是通过不断修改`indicatorHolder` 的属性（调用`indicatorHolder` 的 setXXX() 而 setXXX()方法中会调用 invalidate() 重绘 view）实现动画的。具体可参见：[ValueAnimator和ObjectAnimator的高级用法](http://blog.csdn.net/jdsjlzx/article/details/45558901)
``` java
public class IndicatorHolder {
        private int centerX;
        private int centerY;
        private int height;
        private int color;
        private int width;
        private int alpha;

        public void setAlpha(int alpha) {
            this.alpha = alpha;
            invalidate();
        }

        public int getAlpha() {

            return alpha;
        }

        public void setHeight(int height) {
            this.height = height;
            invalidate();
        }
        ...........
```
<br>

- 注意 getHeight 方法在 onMeasure 中调用的话获取到的值是不正确的，因为此时视图的高和宽还在计算。
``` java

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);
        //getHeight方法在onDraw方法中会取到错误的值
        if (indicatorHolder != null) {
            indicatorHolder.setColor(mIndicatorColor);
            indicatorHolder.setCenterX(mIndicatorPos * mLineWidth + getPaddingLeft() + mIndicatorSize / 2);
            indicatorHolder.setCenterY(getHeight() / 2);
            indicatorHolder.setHeight(mIndicatorSize);
            indicatorHolder.setWidth(mIndicatorSize);
            indicatorHolder.setAlpha(255);
        }
    }

```

##### **`onDraw()`方法**
绘制控件的关键方法<br>
1 控件中包含多个圆，使画笔支持抗锯齿使视觉效果更好 `setAntiAlias(true)`
2 画线的时候先判断是否设置了 `线段不可见` 属性
3 画 `小圆点` ，关键的一句 ` if (switchTo != -1 && i == switchTo)`：

- switchTo 的值默认为 -1 ，在 onTouchEvent 事件中会为其赋值，当 `小圆点` 被点击或 `指示点` 被拖拽
 -  `同时被点击小圆点的位置与当前正在绘制的小圆点的位置相同` 或
 -   `指示点拖拽时指示点中心点（椭圆外切矩形对角线交点）所在区域（由 clickableAreas 规定的区域）  与 当前正在绘制的小圆点的位置相同`  时该 if 才为 true
- switchTo 会在 `animEnd()` 方法中被重置为 -1
-  话小圆点时为 `clickableAreas` 赋值，记录当前小圆点的圆心坐标

4 画指示点：

- 通过调用 `indicatorHolder` 的 `getXXX()`方法获得指示点的当前形态进行绘制
- 属性动画在调用了 `start()`之后会不断调用 `indicatorHolder` 的 `setXXX()`方法，同时调用 `invalidate`方法，`onDraw(...)`方法就会被不断调用，视图不断刷新，指示点、小圆点以及线段的形态就会不断改变，动画就形成了。
``` java

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        //去锯齿
        mPaint.setAntiAlias(true);

        //画线（如果可见）
        if (mLineVisible) {
            mPaint.setColor(mLineColor);
            for (int i = 0; i < mDotCount - 1; i++) {
                int left = getPaddingLeft() + mIndicatorSize / 2 + mLineWidth * i;
                int top = (getHeight() - mLineHeight) / 2;
                int right = getPaddingLeft() + mIndicatorSize / 2 + mLineWidth * (i + 1);
                int bottom = (getHeight() + mLineHeight) / 2;
                canvas.drawRect(left, top, right, bottom, mPaint);
            }
        }

        //画小圆点
        for (int i = 0; i < clickableAreas.length; i++) {
            int cx = i * mLineWidth + getPaddingLeft() + mIndicatorSize / 2;
            int cy = getHeight() / 2;
            if (switchTo != -1 && i == switchTo)
                mPaint.setColor(mIndicatorColor);
            else
                mPaint.setColor(mDotColor);
            canvas.drawCircle(cx, cy, mDotSize, mPaint);
            clickableAreas[i][0] = cx;
            clickableAreas[i][1] = cy;
        }

        //画指示点
        mPaint.setColor(indicatorHolder.getColor());
        mPaint.setAlpha(indicatorHolder.getAlpha());
        canvas.drawOval(
                indicatorHolder.getCenterX() - indicatorHolder.getWidth() / 2,
                indicatorHolder.getCenterY() - indicatorHolder.getHeight() / 2,
                indicatorHolder.getCenterX() + indicatorHolder.getWidth() / 2,
                indicatorHolder.getCenterY() + indicatorHolder.getHeight() / 2,
                mPaint
        );
    }
```

##### 最后一个关键方法 `onTouchEvent`
1 `touchEnable`属性设为 false 则不需要响应触摸事件
2 动画正在进行时不再响应触摸事件，否则会乱套.....
3 `if (switchTo != mIndicatorPos && !mDotClickEnable && !haveIndicatorDraged)` ：

- 点击了非指示点所在的小圆点 && 不可点击 && 指示点没有被拖拽：脑补一下这种情形就是——当你设置了不可点击（点击非指示点所在小圆点时不移动指示点到所点击的小圆点的位置上），但你点击了，而且点击的不是指示点所在的小圆点。此时就不要移动指示点了，因为设置了 不可点击嘛。
- 有没有疑惑为什么要加一个 `&& !haveIndicatorDraged`的条件？往下看...
- 之所以要加这个判断是当你设置了 `不可点击`，`可拖拽` 时，当你开始拖拽，而且拖拽位置超出当前指示点范围（`clickableAreas`规定的范围），假设此时如果只判断 `switchTo != mIndicatorPos && !mDotClickEnable`，那么此时这两条件都满足，返回。那此时效果就是指示点不能拖出`clickableAreas`规定的范围，显然不满足 `不可点击`，但`可拖拽`的要求，所有还要加一个 `是否被拖拽过的条件`。

4 接下来依次判断手势状态

- 按下：按在指示点上还是其他小圆点上
- 抬起：按下的点不是当前指示点所在小圆点（按下后立刻抬起），或被拖拽过
- 挤压：手指一直与屏幕接触，这个时候就是拖拽了
``` java
 @Override
    public boolean onTouchEvent(MotionEvent event) {

        if (!mTouchEnable)
            return true;

        //动画正在进行时不在响应点击事件
        if (haveIndicatorAniming)
            return true;

        int ex = (int) event.getX();
        int temp = mLineWidth / 2;
        switchTo = 0;
        //判断当前手指所在的小圆点是哪个
        for (; switchTo < mDotCount; switchTo++) {
            int[] xy = clickableAreas[switchTo];
            //只对x坐标位置进行判断，这样即使用户手指在控件外面（先在控件内触摸后不抬起而是滑到控件外面）滑动也能判断
            if (ex <= xy[0] + temp && ex >= xy[0] - temp) {
                break;
            }
        }

        if (switchTo != mIndicatorPos && !mDotClickEnable && !haveIndicatorDraged)
            return true;

        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            //按下且不是指示点所在的小圆点
            if (mIndicatorPos != switchTo) {
                startSwitchAnimation();
                if (mListener != null)
                    mListener.onDotClickChange(this, switchTo);
            } else {//按下且是指示点所在的小圆点
                if (mIndicatorDragEnable)
                    startPressAnimation();
            }
        } else if (event.getAction() == MotionEvent.ACTION_UP) { //手抬起
            if (switchTo != mIndicatorPos || haveIndicatorDraged) {
                haveIndicatorDraged = false;
                if (mIndicatorDragEnable)
                    startSwitchAnimation();
            }
        } else { //按着+拖拽
            if (mIndicatorDragEnable) {
                haveIndicatorDraged = true;
                indicatorHolder.setCenterX(ex);
            }
        }

        return true;
    }
```
##### 其他方法
>代码有些多又繁琐就没贴完，上传到GitHub了，可以在这里下载到源码和示例：[DuanJiaNing/IndicatorViewDemo](https://github.com/DuanJiaNing/IndicatorViewDemo)

-  开始指示点触摸（挤压）动画：`startPressAnimation()`
-  开始指示点切换动画： `startSwitchAnimation() `
-  指示点切换动画结束或取消时重置和恢复一些变量的值：`animEnd()`
``` java
/**
     * 指示点触摸（挤压）动画
     */
    private void startPressAnimation() {
		........
    }

    /**
     * 指示点切换动画
     */
    private void startSwitchAnimation() {

        //平移
        int startX = indicatorHolder.getCenterX();
        int endX = switchTo * mLineWidth + getPaddingLeft() + mIndicatorSize / 2;
        ValueAnimator trainsAnim = ObjectAnimator.ofInt(indicatorHolder, "centerX", startX, endX);
        trainsAnim.setDuration(mDuration);

        tempLineColor = mLineColor;
        AnimatorSet defaultIndicatorSwitchAnim = new AnimatorSet();
        defaultIndicatorSwitchAnim.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {
                mLineColor = indicatorHolder.getColor();
                haveIndicatorAniming = true;
            }

            @Override
            public void onAnimationEnd(Animator animation) {
                animEnd();
            }

            @Override
            public void onAnimationCancel(Animator animation) {
                animEnd();
            }

            @Override
            public void onAnimationRepeat(Animator animation) {

            }
        });

        if (mSwitchAnimator == null) {
            switch (mIndicatorSwitchAnim) {
                case INDICATOR_SWITCH_ANIM_NONE:
                    indicatorHolder.setCenterX(endX);
                    animEnd();
                    break;
                case INDICATOR_SWITCH_ANIM_SQUEEZE:
                    //“挤扁”
                    int centerH = mLineHeight * Math.abs(switchTo - mIndicatorPos);
                    int centerW = Math.abs(indicatorHolder.getCenterX() - clickableAreas[switchTo][0]);
                    ValueAnimator heightAnim = ObjectAnimator.ofInt(indicatorHolder, "height", mIndicatorSize, centerH, 0);
                    ValueAnimator widthAnim = ObjectAnimator.ofInt(indicatorHolder, "width", mIndicatorSize, centerW, 0);
                    heightAnim.setDuration(mDuration);
                    widthAnim.setDuration(mDuration);

                    //缩放
                    ValueAnimator scaleAnimH = ObjectAnimator.ofInt(indicatorHolder, "height", mDotSize, mIndicatorSize);
                    ValueAnimator scaleAnimW = ObjectAnimator.ofInt(indicatorHolder, "width", mDotSize, mIndicatorSize);
                    AnimatorSet scaleSet = new AnimatorSet();
                    scaleSet.play(scaleAnimH).with(scaleAnimW);
                    scaleSet.setDuration(500);

                    defaultIndicatorSwitchAnim.play(trainsAnim).with(heightAnim).with(widthAnim);
                    defaultIndicatorSwitchAnim.play(scaleSet).after(trainsAnim);
                    defaultIndicatorSwitchAnim.start();
                    break;
                case INDICATOR_SWITCH_ANIM_TRANSLATION:
                    defaultIndicatorSwitchAnim.play(trainsAnim);
                    defaultIndicatorSwitchAnim.start();
                    break;
            }

        } else { //自定义
            tempLineColor = mLineColor;
            AnimatorSet customAnim = mSwitchAnimator.onIndicatorSwitch(this, indicatorHolder);
            customAnim.play(trainsAnim);
            customAnim.addListener(new Animator.AnimatorListener() {
                @Override
                public void onAnimationStart(Animator animation) {
                    mLineColor = indicatorHolder.getColor();
                    haveIndicatorAniming = true;
                }

                @Override
                public void onAnimationEnd(Animator animation) {
                    animEnd();
                }

                @Override
                public void onAnimationCancel(Animator animation) {
                    animEnd();
                }

                @Override
                public void onAnimationRepeat(Animator animation) {

                }
            });
            customAnim.start();
        }

    }

    /**
     * 指示点切换动画结束或取消时重置和恢复一些变量的值
     */
    private void animEnd() {
        mLineColor = tempLineColor;
        mIndicatorPos = switchTo;
        switchTo = -1;
        haveIndicatorAniming = false;
    }
```
##### 在Activity中设置自定义动画和点击事件监听以及一些属性的修改和获取

[DuanJiaNing/IndicatorViewDemo](https://github.com/DuanJiaNing/IndicatorViewDemo) 中示例Activity的位置：
`\src\main\java\com\duan\indicatorviewdemo\MainActivity.java`
``` java
public class MainActivity extends AppCompatActivity {
	private IndicatorView indicator;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        indicator = (IndicatorView) findViewById(R.id.indicator);
        
        indicator.setOnDotClickListener((View v, int position) -> Toast.makeText(this, "点击了 " + position, Toast.LENGTH_SHORT).show());
        indicator.setOnIndicatorSwitchAnimator((IndicatorView view, IndicatorView.IndicatorHolder target) -> {

            int terminalColor = indicator.getIndicatorColor();
            int centerColor = indicator.getDotColor();
            ValueAnimator colorAnim = ObjectAnimator.ofArgb(target, "color", terminalColor, centerColor, terminalColor);

            int terminalSize = indicator.getIndicatorPixeSize();
            int centerSize = indicator.getIndicatorPixeSize() * 3 / 2;
            ValueAnimator animatorH = ObjectAnimator.ofInt(target, "height", terminalSize, centerSize, terminalSize);
            ValueAnimator animatorW = ObjectAnimator.ofInt(target, "width", terminalSize, centerSize, terminalSize);

            AnimatorSet set = new AnimatorSet();
            set.play(colorAnim).with(animatorH).with(animatorW);
            set.setDuration(500);

            return set;
        });
        //indicator1.setIndicatorSwitchAnim(random.nextInt(IndicatorView.INDICATOR_SWITCH_ANIM_TRANSLATION);
        ...
	}
```
##### 注意点
- 自定义属性的使用，在构造方法中用`context.getTheme().obtainStyledAttributes(...)`方法获取
- `onMeasure()`方法中对宽高的计算
- `onDraw`绘制圆、矩形、椭圆时坐标的确定
- 重写触摸事件时对各种状态的判断
- `animEnd`：动画结束或取消时重置和恢复一些变量的值
- 即使设置`lineVisible`为 false ，也要为`lineWidth`赋值，当然也可以使用默认的，因为`lineWidth`是`onMeasure`方法计算控件`width`的重要变量。
- 调用`setOnIndicatorSwitchAnimator`或`setOnIndicatorPressAnimator`自定义动画时，要将定义好的`AnimatorSet`动画作为返回值返回，由控件控制动画在何时播放和添加监听事件。

---
你可以为`IndicatorHodler`添加更多属性并修改`onDraw()`方法以实现更丰富的动画
源码和示例已上传GitHub，可以在这里下载到：[DuanJiaNing/IndicatorViewDemo](https://github.com/DuanJiaNing/IndicatorViewDemo)
<font size="2" color="#a8a8a8">觉得还不错的话就给颗star吧>.<</font>

------
