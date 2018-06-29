![](http://upload-images.jianshu.io/upload_images/7460499-049618bfd01c3250.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
#### 1. left，top，right，bottom

View 的位置主要由它的四个顶点的位置来决定，分别对应 View 的四个属性：left top right bottom。

- left: View 左上顶点相对于父容器的横坐标
- top: View 左上顶点相对于父容器的纵坐标
- right: View 右下顶点相对于父容器的横坐标
- bottom: View 右下顶点相对于父容器的纵坐标

注意这几个属性值的决定都是参照父容器的坐标。其值的设置和获取可通过对应的 getter/setter 方法。

#### 2. X，Y

从 Android 3.0 开始，View 增加了 x，y，translationX 和 translationY。
x，y 同样是 View 左上角相对父容器的坐标，但不同于 left 和 top ，这两个坐标点的值并一定都是相等的。而不相等的情况是由 translationX 和 translationY 值的设置引起的。

#### 3. translationX，translationY

translationX 和 translationY 是 View 左上角相对父容器左上角的偏移量，translationX 和 translationY 默认值都为 0。


> 几个参数有如下换算关系：
> x = left + translationX

#### 4.总结

left/top，right/bottom 的值都是相对父容器而言的，具体为父容器的左上角，而 translationX/Y 可以移动这个参照点（父容器左上角），通过改变参照点的位置来改变 View 的位置。

这里要注意的是 translationX/Y 改变参照点的位置是理论上的改变，只是子 View 参照的位置变了，父容器左上角的实际坐标是没有变化的。

在这种情况下（translationX/Y 均不为 0，即子 View 的参照点位置变了），x/y 和 left/top 的值就不相等了，此时 x/y 的值是参照父容器左上角实际位置的坐标，而 left/top 是参照 translationX/Y 变化后的坐标点的值。

通过上面的分析可以分析这样一种情况：
<u>**改变 View 的 translationX/Y，此时 View 的 x/y 变化，而 left/top 不变。**</u>

写个 demo 验证下：

布局
```xml

...
    <FrameLayout
        android:layout_margin="30dp"
        android:layout_width="match_parent"
        android:layout_height="match_parent"
        >
        <TextView
            android:id="@+id/text_test"
            android:textColor="@color/colorAccent"
            android:padding="20dp"
            android:background="@color/colorPrimaryDark"
            android:text="sdafasd"
            android:layout_width="wrap_content"
            android:layout_height="wrap_content"
            />
    </FrameLayout>
...    
```

代码

```java

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);

        TextView test = (TextView) findViewById(R.id.text_test);
        test.setTop(10);
        test.setLeft(30);
        out(test);

        test.setTranslationX(50);
        test.setTranslationY(80);
        out(test);

    }

    private void out(View view) {
        int top = view.getTop();
        int left = view.getLeft();
        int x = (int) view.getX();
        int y = (int) view.getY();
        int translationX = (int) view.getTranslationX();
        int translationY = (int) view.getTranslationY();

        StringBuilder builder = new StringBuilder();
        builder.append(" top=" + top)
                .append(" left=" + left)
                .append(" x=" + x)
                .append(" y=" + y)
                .append(" translationX=" + translationX)
                .append(" translationY=" + translationY);

        Log.i("main", builder.toString());

    }
```

日志输出

```powershell
07-17 01:54:33.836 9212-9212/com.duan.myapplication I/main:  top=10 left=30 x=30 y=10 translationX=0 translationY=0
07-17 01:54:33.840 9212-9212/com.duan.myapplication I/main:  top=10 left=30 x=80 y=90 translationX=50 translationY=80
```

分析：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-569b424b99e09128?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

-----------
