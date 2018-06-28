###### 为媒体资源播放打造的“条形与波浪”可视化效果


<hr>

#### 1 截图

![](http://upload-images.jianshu.io/upload_images/7460499-0cecb9c4e0316e7b.gif?imageMogr2/auto-orient/strip)

#### 2 说明

1 提供的 xml 属性

- barColor  横条颜色
- barHeight  横条高度
- waveRange  波浪条极差（最高与最低的差值）
- waveMinHeight  
- waveColor  波浪条最小高度
- waveWidth  波浪条颜色
- waveNumber  波浪条宽度
- waveInterval  波浪条数量

![](http://upload-images.jianshu.io/upload_images/7460499-db6a8b7a4363d0fd.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

2 主要方法

1. `setFallAutomaticallyEnable(boolean enable)`
设置波浪条高度大于 waveMinHeight 时自动坠落（使用属性动画）。
2. `setWaveHeight(float[] hs)`
设置波浪条的高度，高度值是 0 ~ 1 的一个浮点数，即所占 waveRange 的百分比，为 0 时波浪条的高度为 waveMinHeight，为 1 时波浪条的高度为 waveMinHeight + waveRange。
3. `setWaveColor(int[] color)`
设置波浪条的颜色，此方法为每一个波浪条设置指定的颜色。
4. `setWaveColor(int[][] color)`
设置波浪条的颜色，为每一个波浪条指定两个颜色，这两个颜色将以纵向渐变的形式被绘制。
5. `setWaveColor(int color)`
设置波浪条的颜色，让所有的波浪条都显示相同的颜色。
6. `setBarColor(int color)`
设置横条的颜色。
7. `setFallDuration(int duration)`
设置“自动坠落”时的动画时长，如果“自动坠落”未启用，该方法将启动“自动坠落”。
#### 3 如何使用
复制 library 下的 BarWavesView.java 文件到你的项目中，注意修改包名，同时复制 library 目录下的 attrs 中的属性到你自己的 attrs 中（如果没有 attrs
文件，则直接复制文件）。

在 xml 中使用：
```xml
<!--记得修改包名 --!>
<com.duan.library.BarWavesView
    android:id="@+id/BarWavesView_3"
    android:layout_width="wrap_content"
    android:layout_height="wrap_content"
    android:layout_marginTop="-230dp"
    app:waveNumber="35"
    app:waveWidth="30dp"
    app:waveRange="600dp"
    app:waveMinHeight="0dp"
    app:waveInterval="5dp"
    app:waveColor="#7eaeaeae"
    app:barHeight="0dp"
/>

```

在 java 中使用：
```java
//省略代码
public class MainActivity extends AppCompatActivity {
    private BarWavesView barWavesView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        barWavesView = (BarWavesView) findViewById(R.id.BarWavesView);
        barWavesView.setBarColor(ColorUtils.getRandomColor()); // ColorUtils.getRandomColor() 获得一个随机颜色 
        
        int[][] cs = new int[barWavesView.getWaveNumber()][2];
        for (int i = 0; i < cs.length; i++) {
            // 控件允许给每一条波浪条单独设置颜色，这两个颜色将以纵向渐变的形式被绘制
            cs[i][0] = ColorUtils.getRandomColor(); 
            cs[i][1] = ColorUtils.getRandomColor();
        }
        barWavesView.setWaveColor(cs);
        
        // barWavesView.setWaveHeight(float[] hs); 修改控件波浪条高度
        
    }
}
//省略代码
```
详见 app 模块示例。

#### 4 原理解析

##### 4.1 onMeasure

onMeasure 方法用于测量控件的宽高和调整控件的属性大小；

- 当在 xml 中指定控件宽或高为 warp_content 时，宽和高将通过如下方式计算得出：
`width = mWaveWidth * mWaveNumber + mWaveInterval * (mWaveNumber - 1);`
`height = mWaveMinHeight + mWaveRange + mBarHeight;`

- 当在 xml 中指定控件宽或高为 march_parent 或具体数值时，宽和高将通过相应的调整策略进行调整以适应指定的宽高（大多数情况下控件使用者是无法在固定宽高的情况下合理分配，计算波浪条的个数、宽度及间隙以适合于当前的宽高，从而使控件呈现的效果违背控件设计的初衷），调整的方法是对 xml 中指定的属性或默认属性进行调整，使其在最小修改指定属性的前提下适合于宽高。
宽度的调整方法：
```java
private void adjustWidth(int width) {

        while (width < mWaveInterval * (mWaveNumber - 1) + mWaveWidth * mWaveNumber) {
            if (mWaveInterval > sMIN_WAVE_INTERVAL) {
                mWaveInterval--; // 首选调整波浪条间距
            } else {
                if (mWaveWidth > sMIN_WAVE_WIDTH) {
                    mWaveWidth--; // 其次选择调整波浪条宽度
                } else {
                    width++; // 万不得已选择调整设置的宽度
                }
            }
        }

    }
```
高度的调整方法：
```java
private void adjustHeight(int height) {

        while (mWaveMinHeight + mWaveRange + mBarHeight > height) {
            if (mBarHeight > sMIN_BAR_HEIGHT) {
                mBarHeight--; //首选调整横条高度
                continue;
            }

            if (mWaveMinHeight > sMIN_WAVE_HEIGHT) {
                mWaveMinHeight--; // 其次选择调整波浪条的最小高度
                continue;
            }

            if (mWaveRange > sMIN_WAVE_RANGE) {
                mWaveRange--; // 再次选择调整波浪条极差
            }
        }
    }
```

- 控件并未适配 padding 和 min/maxHeight 等属性

##### 4.2 onLayout

onLayout 方法用于确定控件在其父控件中的位置，并没有对该方法进行覆写。

##### 4.3 onDraw

onDraw 方法负责绘制控件，在该控件中主要需要绘制两部分：波浪条，横条。

- 绘制横条

```java
private void drawBar(Canvas canvas) {
        mPaint.setShader(null); // 绘制波浪条使会使用到渐变着色器，这里要将着色器清除
        mPaint.setAlpha(255); // 绘制波浪条使颜色可能带有 alpha 层，这里要将 alpha 清除，否则 alpha 值会遗留下来，干扰横条的绘制，由 mBarColor 决定最终的颜色
        mPaint.setColor(mBarColor);
        float right = mWaveInterval * (mWaveNumber - 1) + mWaveWidth * mWaveNumber; //横条的长度由波浪条的属性决定
        float top = getHeight() - mBarHeight;
        canvas.drawRect(0, top, right, getHeight(), mPaint);
    }
```

- 绘制波浪条
```java
private void drawWaves(Canvas canvas) {

        for (int i = 0; i < mWaveNumber; i++) {
            float left = mWaveWidth * i + mWaveInterval * i;
            float right = left + mWaveWidth;

            float bottom = getHeight() - mBarHeight;
            float fs = mWaveHeight[i];
            float top;
            top = bottom - mWaveMinHeight - (fs * mWaveRange); // 由百分比计算出需要绘制的高度，再计算其 top 坐标值。
            LinearGradient lg = new LinearGradient(
                    left, bottom,
                    right, top,
                    mWaveColors[i],
                    null,
                    Shader.TileMode.CLAMP
            );
            mPaint.setAlpha(255);
            mPaint.setShader(lg);
            canvas.drawRect(left, top, right, bottom, mPaint);
        }

    }
```

##### 4.5 修改波浪条的高度

修改波浪条高度时只需传入新的波浪条高度的百分比数组，如果“自动坠落”动画启用，则先要停止动画，重绘之后再开始动画。
```java
public void setWaveHeight(float[] hs) {
        if (mFallAnimEnable && mAnim != null && (mAnim.isStarted() || mAnim.isRunning())) {
            mAnim.cancel();
        }

        setWaveHeights(hs);
        invalidate();

        if (mFallAnimEnable) {
            if (mAnim == null) {
                initAnim(mFallDuration);
            }
            mAnim.start();
        }
    }
    
    ...
    
    private void setWaveHeights(float[] hs) {
        if (hs == null || hs.length != mWaveNumber) {
            return;
        }
        mWaveHeight = hs;
    }
```

##### 4.6 "自动坠落"动画

“自动坠落”动画的初始化如下：
使用属性动画，在动画属性值更新时遍历波浪条，修改波浪条的高度占比从当前值递减到 0，注意`mWaveHeight`为波浪条高度中大于最小高度而小于最小高度加上极差的部分的占比数组（波浪条高度的计算方式为：mMinWaveHeight + mWaveRange * mWaveHeight[i]）

```java
private void initAnim(int dur) {
        mAnim = ObjectAnimator.ofFloat(1.0f, 0.0f);
        mAnim.setInterpolator(new AccelerateDecelerateInterpolator());
        mAnim.setDuration(dur);
        mAnim.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                float v = (float) animation.getAnimatedValue();
                for (int i = 0; i < mWaveHeight.length; i++) {
                    mWaveHeight[i] *= v;
                }
                invalidate();
            }
        });
    }
```

GitHub 地址：[DuanJiaNing/BarWavesVew](https://github.com/DuanJiaNing/BarWavesVew)

---
