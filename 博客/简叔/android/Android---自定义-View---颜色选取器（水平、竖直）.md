![](http://upload-images.jianshu.io/upload_images/7460499-1cae50e7ad4dfb10.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
Android 自定义 View - 颜色选取器（水平、竖直）

### Android 自定义 View - 颜色选取器（水平、竖直）

类似 SeekBar 的方式通过滑动选择颜色。

#### 效果图

![](https://raw.githubusercontent.com/DuanJiaNing/ColorPicker/master/record.gif)

#### xml 属性

1. indicatorColor 指示点颜色
2. indicatorEnable 是否使用指示点
3. orientation 方向
horizontal 水平
vertical 竖直

#### 使用

复制 \library\src\...\ColorPickerView.java 和 \library\src\main\res\values\attrs.xml 文件到你的项目中，就可以在使用啦。

示例：
在 xml 中使用：

```xml

    <com.duan.colorpicker.ColorPickerView <!--替换包名-->
        android:layout_width="50dp"
        android:layout_height="200dp"
        app:indicatorEnable="true"
        app:indicatorColor="#fff"
        app:orientation="vertical" />

```

在 java 中使用：

```java
...
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ColorPickerView picker = (ColorPickerView) findViewById(R.id.colorPickerView);
        picker.setIndicatorColor(Color.GREEN);
        picker.setOrientation(ColorPickerView.Orientation.HORIZONTAL);
        picker.setColors(Color.DKGRAY,Color.RED,Color.WHITE);
        picker.setOnColorPickerChangeListener(new ColorPickerView.OnColorPickerChangeListener() {
            @Override
            public void onColorChanged(ColorPickerView picker, int color) {
                // TODO
            }

            @Override
            public void onStartTrackingTouch(ColorPickerView picker) {
                // TODO

            }

            @Override
            public void onStopTrackingTouch(ColorPickerView picker) {
                // TODO

            }
        });
        }
...
```

### 实现解析

#### 1 构成

* 指示点：类似于 SeekBar 的滑块，通过滑动指示点来选取颜色
* 颜色条：放置可选颜色

![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-44273af8894a257b?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

颜色条通过 Paint 的 setShader 方法，使用 LinearGradient 渐变色着色器绘制。
指示点只是普通的圆，不过加了阴影，使用 Paint 的 setShadowLayer 方法实现，使用该方法时要关闭硬件加速。
#### 2 实现逻辑

`public class ColorPickerView extends View`
控件继承自 View。

##### 2.1 onMeasure

onMeasure 方法完成控件大小的测量。控件定义了最小宽高，所以当指定控件宽高，且指定值小于最小宽高，则指定无效。

##### 2.2 onLayout

onLayout 方法比较关键，在该方法中需要完成如下的任务：
1. 计算出控件可用空间
2. 初始化指示点的坐标
3. 计算出颜色条的边界
4. 设置颜色条的颜色（默认的渐变色）
4. 初始化两张 Bitmap（一张用于绘制颜色条，一张用于绘制指示点）

###### 2.2.1 指示点坐标的确定：
初始化时默认使指示点位于控件的中心，而后其位置由 onTouchEvent 方法控制，同时在 onTouchEvent 方法中进行重绘通知以及当前颜色选取。

###### 2.2.2 颜色条边界确定：
颜色条和指示点的大小比例计算方式：我将控件的可用空间（除去上下左右 padding 后剩余的空间）分为 9 份，这 9 份的分配方式是这样的：
假设控件此时为水平方向，且宽度大于高度（这是一般的情况，在控件方向为水平，宽度小于高度时的情况下，边界要进行特殊计算；控件方向为竖直，宽度大于高度的情况也需要特殊处理），取高度作为基数（取宽高中短的一边作为基数）进行平均分配，即： 高度 / 9 = 每一份的大小。

1/9 留白
2/9 指示点在颜色条上方的部分
3/9 颜色条高度
2/9 指示点在颜色条下方的部分
1/9 留白

这样分之后就可以得出 圆的直径占有 9 份中的 7 份，颜色条占有 3 份，两份留白，这是高度的分配情况；颜色条的宽度满足如下条件：在可用宽度的基础上，左右分别留出指示点半径的宽度，这是为了在指示点滑动到左右端点时留出空间给指示点显示，同时保证指示点圆心能完整的扫过整个颜色条。竖直方向的测量计算逻辑也是一样的。

特殊情况：
有两种情况需要特殊处理
1. 控件为水平方向，此时控件的可用宽度小于可用高度。
2. 控件为竖直方向，此时控件的可用宽度大于可用高度。

这两种情况的处理逻辑是一样的，拿第一种情况举例，若此时仍然以短边（此时为宽）作为基数分为 9 份计算，左右分别留出圆半径的宽度，此时圆直径占有了控件可用宽的 7 / 9 ，而且颜色条左右分别留出 3.5 / 9（指示点半径） 的空间，那么颜色条的宽度只剩 9 / 9 - ( 7 / 9) = 2/ 9 ，2 / 9 < 7 / 9，而且颜色条和指示点都是居中显示的，这就导致指示点大部分遮挡甚至完全遮挡住颜色条。

解决方法：
针对第一种情况，此时使均分为 9 份的基数为宽（短边）的 1 / 6（控件默认有个最小宽高，默认值的长边与短边之比就是 6 ： 1）。
第二种情况下，使基数为高度的 1 / 6。

不足：
假设控件为水平方向，此时控件的可用宽度大于可用高度，但宽与高差值很小。这种情况下，指示点仍然有可能大部分遮挡甚至完全遮挡住颜色条，这种情况下并没有进行处理，此时只能由使用者进行控制。经过测试，在这种情况下（水平方向，宽大于高），当宽高比大于 3 ： 1 时，显示效果比较好，所以应该尽量让宽高比大于 3 ： 1。竖直方向有同一的问题，不同的是，此时应尽量使高与宽的比值大于 3 ： 1.


###### 2.2.3 为什么使用两张 Bitmap

onDraw 方法并不是直接绘制圆角矩形，然后绘制指示点（圆），这样做会使两部分直接绘制在一张位图上，相互覆盖，不利于取得当前指示点所指颜色。因而使用两张位图，一张负责绘制颜色条，一张绘制指示点，onDraw 时分别绘制这两张位图，取色时获取颜色条对应位图上像素点的颜色即可。

取得位图上指定点颜色的方法是使用 Bitmap 的 getPixel(int x,int y) 方法，这个方法可以取得位图上由 x，y 指定的点像素，根据这个像素可以解析出这个点的颜色。

同时这样可以提高控件绘制效率，在大多数情况下颜色条上的可选颜色是不会变化的，此时可以将在可选颜色发生变化后生成的位图直接绘制到控件上，而不需要再一次绘制这个位图，指示点也如此，只需在选取颜色时（滑动指示点时）改变绘制指示点位图的坐标即可，无需再次生成指示点的位图。


##### 2.3 onDraw

onDraw 方法负责绘制，绘制时判断指示点对应位图和颜色条对应位图是否需要重绘，需要则重绘，后绘制两张位图到控件上，否则直接绘制两张位图到控件。


代码中写了很多注释，可以参照注释理解。

已上传 Github，可以在这里找到：[DuanJiaNing/ColorPicker](https://github.com/DuanJiaNing/ColorPicker)
