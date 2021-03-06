## finalize() 方法

finalize 方法定义在 Object 类中，其方法签名如下：
```java
protected void finalizer() throw Throwable { }
```

1. finalize 方法在 GC（垃圾回收器）决定回收一个不被其他对象引用的对象时调用。子类覆写 finalize 方法来处置系统资源或是负责清除操作。

2. finalizer 方法的调用时机由 sun 和 JVM 开发商共同决定：确定不再有**任何未死亡的线程**通过**任何方法**调用来访问或使用该对象时。（即确定对象的任何方法都不（再）会被调用时，调用其 finalize 方法）

3. 除非一些其他的已经准备好被终止的对象或类将调用 finalize 方法包括在其终止动作之中。（即调用对象的 finalize 方法，此时该对象的 finalize 方法将是最后被调用的方法，*在这之后,对象的任何方法都不（再）会被调用。*

4. finalize 方法中可以执行任何操作，包括再次使该对象可用于其它线程（重新初始化）；但是，finalize 的通常目的是在对象（一定）不再被需要时（对象将被丢弃）**之前**执行清除操作。例如，表示输入/输出连接的对象的 finalize 方法可能会在对象被永久丢弃之前执行显式 I/O 事务来中断连接。

5. Object 类没有提供 finalize 方法的实现，导出类可以按需覆写该方法。

6. java 不对任何对象的 finalize 方法调用发生的线程做限制，即任何线程都可以调用对象的 finalize 方法，然而，调用 finalize 方法的线程将不能持有任何*用户可见的线程同步锁*。当 finalize 方法被调用时，如果 finalize 方法抛出异常，且异常未被捕获时，异常将被忽略，finalize 方法将中止。

7. 当对象的 finalize 方法被调用后，不会再有基于该对象的方法调用，直到 JVM 再次进行回收动作时该对象将被释放，占用的内存将被回收。
8. 任何对象的 finalize 方法只会被 JVM 调用一次。

9. finalize（）方法引发的任何异常都会导致该对象的终止被暂停，否则被忽略。

-----
