![](http://upload-images.jianshu.io/upload_images/7460499-0595fa7565ccaf7f.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
### n 阶贝塞尔曲线计算公式实现

关于贝塞尔曲线是什么，可以用来做什么，这里就不再介绍，如果你还不了解，可以先去看看下面这篇文章：[贝塞尔曲线扫盲](http://blog.csdn.net/cdnight/article/details/48468653)

#### 1. 效果参考
![](http://upload-images.jianshu.io/upload_images/7460499-5966bb5df8ff5f27?imageMogr2/auto-orient/strip)
![](http://upload-images.jianshu.io/upload_images/7460499-939866d360eb8f42?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

#### 2. 思路解析

百度百科上给出的一般参数公式是这样的：
给定点 P0,P1,P2, ... ,Pn，其贝塞尔曲线公式如下（即贝塞尔曲线上的点 B(t) 可由如下公式计算得到）：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-2603066c32c19ba9?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

可以看出其公式是由一个格式固定的表达式之和来表示，这个表达式就是关键：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-1dc858bb0aa50a6a?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
该表达式可分为四个部分看：

- 从 i 递增到 n 的常数部分
- Pi 坐标部分
- (1 - t)^(n - i)
- t^i
可以看出这四部分都与 i 的值相关，此外 t 值的计算方式为：i/(n+1)

##### 如果直接从上面的公式上找规律比较抽象，那就从具体的例子中找规律吧： 

设 Bt 为要计算的贝塞尔曲线上的坐标，N 为控制点个数，P0,P1,P2..Pn 为贝塞尔曲线控制点的坐标，当 N 值不同时有如下计算公式: 
如 N 为 3 表示贝塞尔曲线的控制点有 3 个点，这时 n 为 2 ，这三个点分别用 P0,P1,P2 表示。

- N = 3: P = (1-t)^2*P0 + 2*(1-t)*t*P1 + t^2*P2 <br>
- N = 4: P = (1-t)^3*P0 + 3*(1-t)^2*t*P1 + 3(1-t)*t^2*P2 + t^3*P3 <br>
- N = 5: P = (1-t)^4*P0 + 4*(1-t)^3*t*P1 + 6(1-t)^2*t^2*P2 + 4*(1-t)*t^3*P3 + t^4*P4 <br>

将贝塞尔曲线一般参数公式中的表达式用如下方式表示：
设有常数 a,b 和 c，则该表达式可统一表示为如下形式：
a * (1 - t)^b * t^c * Pn;

分析当 N 分别为3,4,5 时对应 a,b,c 的值： 
如 N = 3 时，公式有三个表达式，第一个表达式为 (1-t)^2*P0，其对应 a,b,c 值分别为：1,2,0 

- N = 3:   1,2,0   2,1,1   1,0,2 
a: 1 2 1 
b: 2 1 0 
c: 0 1 2 
- N = 4:   1,3,0   3,2,1   3,1,2   1,0,3 
a: 1 3 3 1 
b: 3 2 1 0 
c: 0 1 2 3 
- N = 5:   1,4,0   4,3,1   6,2,2   4,1,3   1,0,4 
a: 1 4 6 4 1 
b: 4 3 2 1 0 
c: 0 1 2 3 4 

根据上面的分析就可以总结出 a,b,c 对应的取值规则：

- b: (N - 1) 递减到 0     (b 为 1-t 的幂) 
- c: 0 递增到 (N - 1)     (c 为 t 的幂) 
- a: 在 N 分别为 1,2,3,4,5 时将其值用如下形式表示： <br>
N=1:---------1 
N=2:--------1  1 
N=3:------1  2  1 
N=4:-----1  3  3  1 
N=5:---1  4  6  4  1 
a 值的改变规则为： [杨辉三角](http://baike.baidu.com/item/杨辉三角)

#### 3. 使用 java 来实现
接下来就实现它：先再来一个例子
比如计算控制点坐标分别为：P0(3,8)，P1(2,3)，P2(2,7)，想要返回 10 个在贝塞尔曲线上的点，用 java 可以这样写：
```java
float[] p0 = {3, 8};
        float[] p1 = {4, 3};
        float[] p2 = {2, 7};
        float[][] result = new float[10][2];
        for (int i = 0; i < 10; i++) {
            float t = i / 10;
            result[i][0] = (float) (1 * Math.pow(1 - t, 2) * Math.pow(t, 0) * p0[0] + 2 * Math.pow(1 - t, 1) * Math.pow(t, 1) * p1[0] + 1 * Math.pow(1 - t, 0) * Math.pow(t, 2) * p2[0]);
            result[i][1] = (float) (1 * Math.pow(1 - t, 2) * Math.pow(t, 0) * p0[1] + 2 * Math.pow(1 - t, 1) * Math.pow(t, 1) * p1[1] + 1 * Math.pow(1 - t, 0) * Math.pow(t, 2) * p2[1]);
        }
```
好了，最后的计算方法是下面这个：
```java
     /**
     * @param poss      贝塞尔曲线控制点坐标
     * @param precision 精度，需要计算的该条贝塞尔曲线上的点的数目
     * @return 该条贝塞尔曲线上的点（二维坐标）
	 */
public float[][] calculate(float[][] poss, int precision) {

        //维度，坐标轴数（二维坐标，三维坐标...）
        int dimersion = poss[0].length;

        //贝塞尔曲线控制点数（阶数）
        int number = poss.length;

        //控制点数不小于 2 ，至少为二维坐标系
        if (number < 2 || dimersion < 2)
            return null;

        float[][] result = new float[precision][dimersion];

        //计算杨辉三角
        int[] mi = new int[number];
        mi[0] = mi[1] = 1;
        for (int i = 3; i <= number; i++) {

            int[] t = new int[i - 1];
            for (int j = 0; j < t.length; j++) {
                t[j] = mi[j];
            }

            mi[0] = mi[i - 1] = 1;
            for (int j = 0; j < i - 2; j++) {
                mi[j + 1] = t[j] + t[j + 1];
            }
        }

        //计算坐标点
        for (int i = 0; i < precision; i++) {
            float t = (float) i / precision;
            for (int j = 0; j < dimersion; j++) {
                float temp = 0.0f;
                for (int k = 0; k < number; k++) {
                    temp += Math.pow(1 - t, number - k - 1) * poss[k][j] * Math.pow(t, k) * mi[k];
                }
                result[i][j] = temp;
            }
        }

        return result;
    }
```
在 android 中继承 View 然后重写 onDraw 方法，在 Activity 绑定的布局文件中加入该自定义 View ，调用 calculate 方法就可以画出来任意阶的贝塞尔曲线啦。

```java
........
	// calculate 方法在 BezierImpl 中实现
    private BezierImpl bezier = new BezierImpl();
    private Paint paint = new Paint();
    float[][] poss = {
            {353.0f, 383.0f},
            {670.0f, 266.0f},
            {403.0f, 128.0f},
            {148.0f, 369.0f},
            {400.0f, 513.0f},
            {564.0f, 503.0f},
            {582.0f, 378.0f},
            {682.0f, 878.0f},
            {182.0f, 878.0f}
    };

    @Override
    protected void onDraw(Canvas canvas) {
        float x0, y0, x, y;
        paint.setColor(Color.DKGRAY);
        paint.setStrokeWidth(3.0f);
        x0 = poss[0][0];
        y0 = poss[0][1];
        for (int i = 1; i < poss.length; i++) {
            x = poss[i][0];
            y = poss[i][1];
            canvas.drawLine(x0, y0, x, y, paint);
            x0 = x;
            y0 = y;
        }

        paint.setColor(Color.RED);
        paint.setStrokeWidth(5.0f);
        float[][] po = bezier.calculate(poss, 500);
        x0 = po[0][0];
        y0 = po[0][1];
        for (int i = 1; i < 500; i++) {
            x = po[i][0];
            y = po[i][1];
            canvas.drawLine(x0, y0, x, y, paint);
            x0 = x;
            y0 = y;
        }
    }
    ...........
```
------
最后贴一个最近在做的一个自定义 View（GummyView），使用了到了贝塞尔曲线，用了上面的方法，有兴趣的话可以 Fork 或给我留言 >.<。
地址：[DuanJiaNing/GummyView](https://github.com/DuanJiaNing/GummyView)

目前实现的效果是这样的：
![](https://raw.githubusercontent.com/DuanJiaNing/GummyView/master/gummy_01.gif)
![](https://raw.githubusercontent.com/DuanJiaNing/GummyView/master/gummy_02.gif)
![](http://upload-images.jianshu.io/upload_images/7460499-304fc6c540df54a4.gif?imageMogr2/auto-orient/strip)

--------
