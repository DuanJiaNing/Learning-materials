![](http://upload-images.jianshu.io/upload_images/7460499-1f3d9483d83b1f34.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
原文地址：[用Kotlin开发Android应用](http://www.cnblogs.com/figozhg/p/4983919.html)

### 1 类定义

```kotlin
class Personal(
        var name: String,
        var id: Int = 1,
        var sex: String = "male"
)
```

格式：类名(构造参数…)
构造参数可以指定默认值，那么在初始化实例时，对应位置参数就可以不传值，方法也是，但这里要注意顺序的问题，如 id 不想传值，sex 想传值，这种情况的话似乎不行（就目前学到的关于 Kotlin 的知识来说）
初始化方式有以下几种：
`val per = Personal(12)`
`val per1 = Personal(13,"lili")`
`val per2 = Personal(14,"tom","male")`
另外构造参数同时可以作为该类的属性（域），可以直接操作：
`per.name = "jack"`
`val id = per.id`


### 2 字符串构造

使用 `${}` 嵌入字符串中
`System.out.println("person name is = ${per.name}")`
如果对变量直接引用则只需使用 `$` 后跟变量名即可
```kotlin
fun test(
        var sex = "male"
        System.out.println("person sex is = $sex")
)
```
注意：
当`$sex`紧接着就是字母、数字或下划线时仍需要使用 `{}`


### 3 空类型安全

#### 3.1 使用 `?` 显示指明对象可以为 null

此时要明确指定数据类型
`var sex: String?`
也只有这样，这个参数才能被赋为 null
`var sex: String? = null`
即不能为 null （没有 ? ）的变量是不允许赋 null 的，从而避免遇到非预期的空指针异常

当变量可以为 null ，那么使用该变量的时候 Kotlin 就强制必须判断是否为 null
```kotlin
class MainActivity : AppCompatActivity() {

    var mTextView:TextView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        initViews(null)

    }
    
    private fun initViews(view: View?) {
        mTextView = findViewById(R.id.text_view) as TextView?
        mTextView?.text = "i have change the text"
    }

}
```
注意这一句，如果不加 `?`，编译是无法通过的。
`mTextView?.text = "i have change the text"`

#### 3.2 确定不为 null 才使用，否则抛出异常

`id!!.toString()`
使用 `!!` 代表，表示使用的时候开发者断定此变量一定不会为 null 。但如果开发者错了，变量为 null，那么程序会抛运行时异常而立即终止，开发者需要承担程序终止的风险，所有没十足的把握就尽量不用吧。


#### 3.3 如果为空就给一个值

```kotlin
 fun testNull(id: Int? = null) {
        val caseNull = id?.toString()
        val tr = caseNull ?: "144"
    }
```
id 不为 null 才调用 toString 方法，id 为 null 则 id.toString 也为 null，此时caseNull 也会为 null，要使 caseNull 不为 null，就使用 `?:` 为其指定一个值。
也可以简写为：`val tr = id?.toString()?: "144"`


### 4 扩展函数

可以为任何类添加新函数，只需在函数名前加上类名。在调用处，该函数将被导入到类中。
```kotlin
class Util {
	fun Activity.toast(context: Context, message: CharSequence, duration: Int = Toast.LENGTH_SHORT) {
    Toast.makeText(context, "msg:$message", duration).show()
}
```
可以在 Activity 中这样使用

```kotlin
fun test() {
    val util = Util()
    util.toast(this,"a test msg")

}
```
但不能过度使用，应创建专门的委托类


### 5 函数式支持

```kotlin
fun test() {
	val util = Util()
	val tv = TextView(this)
	tv.setOnClickListener { 
	    var i = 12
	    util.toast(this,"click $i")
	}
}
```

### 6 when 关键字

```kotlin
fun test() {
     var ii = 0
        when (ii) {
            0 -> println( "0：$ii")
            in 1..10 -> println("1-10：$ii")
            else -> println("$ii")
        }
}
```
in：在区间内（闭区间）
else：否则


### 7 open 关键字
一个类想被继承，必须标注为 open 
一个方法想被覆写，父类方法上要加 open，子类覆写时要加 override

```kotlin
open class Util {
    open fun getUtil(): Util {
        return Util()
    }

}
```
```kotlin
class StringUtil : Util() {
    override fun getUtil(): Util {
        return StringUtil()
    }
}
```

### 8 Data class
Java 中我们会创建一些只用于保存数据的实体类

```kotlin
data class User(
        var name: String,
        var id: Int
)
```
hashCode equals toString getter/setter 自动生成


### 9 其它知识点

- 语句末可以（推荐）不使用 ; 号
- 强类型语言：变量类型确定后就不能更改（不能强转）
- var：一般类型
- val：最终类型（只能赋值一次）
- 使用 : 代替 java 中的 extends 关键字，所继承的类型同时要调用其构造方法
- 类型和名称以另一种方式编写，用冒号分割。
- 在Java中，我们可以用注释使我们的代码更清晰，但不是必须的。而Kotlin强制我们这么做。
- 方法参数：类型和名称以另一种方式编写，用冒号分割。
- 显示使用 override 
 
----
