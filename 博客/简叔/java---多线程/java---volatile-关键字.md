> 关键字volatile是java虚拟机提供的**最轻量级的同步机制**。java内存模型对volatile专门定义了一些特殊的访问规则。

## 特性
当一个变量被volatile修饰后，它具备两种特性，第一是保证此变量对所有线程的可见性；第二是禁止指令重拍序。

### 保证可见性，不保证操作原子性
这里的“可见性”值当一个线程修改了这个变量的值，新值对于其他线程来说是可以立即得知的。而普通变量做不到这一点，普通变量的值在线程中传递均需要通过主存来完成。

在各个线程的工作内存中，volatile变量也可以存在不一致的情况，但由于每次使用之前都需要先刷新，执行引擎看不到不一致的情况（不一致是存在的，允许的），因此可以认为不存在一致性问题。**但是java里面的运算并非原子操作，导致volatile变量的运算在并发下一样是不安全（volatile不保证操作原子性）**。

例如定义如下变量：
`private static volatile int va = 0;`
然后开启10个线程，在每个线程中对va累加1000次，那么最终期望的va的值为10000。但由于同步的问题，最终的结果会出现不等于10000的情况（往往小于10000）。
`va++`操作对应的字节码指令如下：
```
1 getstatic
2 iconst_1
3 iadd
4 putstatic
```
可见一个自增操作需要四个字节码指令完成，getstatic用于获得静态变量的值（这里保证了可见性，每次使用都刷新值），2、3两条指令执行值加一操作，putstatic将新增回写（保证可见性）。

volatile不保证操作原子性意味着这4条指令有可能会被中断，如当前va=3，线程A执行到指令2，此时线程A因为某种原因放弃执行权（如Thread.yield()），线程B经调度获得执行权，并且完整的执行4条指令并放弃执行权，此时va=4，线程A重新获得执行权并从上次停下来的地方继续执行，执行2、3指令，此时线程A的工作内存中va的值为4，执行putstatic指令，最终va的值为4。
这里va被两个线程累加了两次，最终值理应为5，但由于不保证操作原子性的关系，最终值为4，小于期望值。

### 禁止指令重拍序
普通的变量仅仅会保证在所有依赖当前值进行赋值的地方都能获取到正确的值，而不保证变量赋值操作的顺序与程序代码中的执行顺序一致。

有如下代码：
```java
Map configOptions;
char[] configText;
// 此变量必须为volatile
volatile boolean initialized = false;

// 假设以下代码在线程A中执行
// 模拟读取配置信息，当读取完成后将initialized设置为true以通知其他线程配置可用
configOptions = new HashMap();
configText = readConfigFile(fileName);
processConfigOptions(configText,configOptions);
initialized = true;

// 假设以下代码在线程B中执行
// 等待initialized为true，代表线程A已经把配置信息初始化完成
while(!initialized){
    sleep();
}
doSomethingWithConfig();

```
如果定义的initialized变量没有使用volatile修饰，就可能会由于指令重排序的优化，导致位于线程A中的最后一句代码`initialized = true;`被提前执行，那么线程B在执行`doSomethingWithConfig()`可能就会出错。

volatile屏蔽指令重拍序的语义在 jdk 1.5 中才被完全修复，此前的JDK中即使变量被声明为volatile也仍然不能完全避免重排序导致的问题。

## 底层实现
被volatile修饰的变量，执行赋值操作后会生成一个内存屏障（Memory Barrier或Memory Fence，指重拍序时不能把后面的指令重拍序到内存屏障之前的位置），只有一个CPU访问内存时，并不需要内存屏障；但如果有两个或多个CPU访问同一块内存，且其中有一个在观测另一个，就需要内存屏障来保证一致性了。

## volatile与锁
在某些情况下，volatile机制的性能确实要优于锁（synchronized或java.util.concurrent包里面的锁），但由于虚拟机对锁实现的许多消除和优化，使得我们很难量化的认为volatile就会比锁快多少。

使用volatile可以确定的一个原则是：**volatile变量的读操作的性能消耗与普通变量几乎没差别，但是写操作可能会慢一些。**因为它需要在本地代码中插入许多内存屏障指令来保证不发生乱序执行。不过即便如此，**大多数场景下volatile的总开销仍然要比锁低。**

----
博文内容摘抄自《深入理解java虚拟机》第12章 Java内存模型与线程 12.3.3: 对于volatile型变量的特殊规则
