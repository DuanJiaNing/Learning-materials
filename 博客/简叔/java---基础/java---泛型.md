![](http://upload-images.jianshu.io/upload_images/7460499-60ded4a99d3ad9ba.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
java - 泛型

### java - 泛型


java SE 5 加入了泛型，泛型实现了 参数化类型 的概念，使代码可以应用于多种类型。

创造容器类是促成泛型出现的原因之一，泛型可以用来指定容器要持有什么类型的对象，而且由编译器来保证类型的正确性。

#### 1. 简单泛型


```java
public class Holder<E> {

    private E ele;

    public Holder(E e) {
        this.ele = e;
    }

    public E get() {
        return ele;
    }

}
```

Holder 类现在可以持有任何类型的对象。如：
```java
    public static void main(String[] args) {
        Holder<String> h1 = new Holder<String>("string");
        Holder<Exception> h2 = new Holder<>(new Exception());
        Exception e = h2.get(); // 取出时自动就是目标类型了
    }
```


#### 2. 泛型接口

定义接口 Generator —— 生成器，专门用于创建对象的类。

```java
public interface Generator<T> {
    T next();
}

```

编写辅助类：Fruit 及其子类 苹果，西瓜，梨。

```java
public class Fruit {
    private static long counter = 0;
    private final long id = counter++; // counter 是属于类的，每次创建对象都会自增

    @Override
    public String toString() {
        String c = getClass().getSimpleName();
        return String.format("%-15s%-10d", c, id); // 格式化字符串
    }

    static class Apple extends Fruit {
    }

    static class Watermelon extends Fruit {
    }

    static class Pear extends Fruit {
    }
}
```

现在可以编写类实现 Generator< Fruit >接口，用于产生水果：

```java
public class FruitGenerator implements 
    Generator<Fruit>, 
    Iterable<Fruit>  { // 使可以在 foreacher 语句中迭代

    private Random random = new Random(47);
    private Class[] types = {Fruit.Apple.class, Fruit.Watermelon.class, Fruit.Pear.class};
    private int size; // 边界值，产生器的生产量边界

    public FruitGenerator(int size) {
        this.size = size;
    }

    @Override
    public Iterator<Fruit> iterator() {
        return new FruitIterator();
    }

    @Override
    public Fruit next() {
        Class type = types[random.nextInt(types.length)];
        Fruit fruit = null;
        try {
            fruit = (Fruit) type.newInstance();
        } catch (InstantiationException | IllegalAccessException e) {
            e.printStackTrace();
        }
        return fruit;
    }

    class FruitIterator implements Iterator<Fruit> { // 迭代器
        int count = size;

        @Override
        public boolean hasNext() {
            return count > 0;
        }

        @Override
        public Fruit next() {
            count--;
            return FruitGenerator.this.next(); // 随机产生[水果]
        }
    }

    public static void main(String[] args) {
        for (Fruit f : new FruitGenerator(10)) {
            System.out.println(f);
        }
    }
}
```

利用反射使随机产生水果。
输出：

```bash
Pear           0         
Pear           1         
Watermelon     2         
Pear           3         
Watermelon     4         
Pear           5         
Watermelon     6         
Pear           7         
Apple          8         
Watermelon     9         

```

泛型的一个局限性：基本类型无法作为类型参数，不过，java SE 5 具备了自动打包和拆包功能，可以很方便的在基本类型与其包装类之间转换。

#### 3. 泛型方法

1. 是否拥有泛型方法与其所在类是否为泛型类没有关系。
2. 使用泛型类或泛型方法的原则：无论何时，只要你做得到，就应该尽量使用泛型方法。
3. 对于一个 static 方法，其无法访问所在泛型类的类型参数，所以，静态方法使用泛型的唯一方式是使其自己成为泛型方法。（类似于类的静态成员和非静态成员的关系，静态成员是属于类的，静态成员无法访问非静态成员，同理，类型参数是属于对象的，静态成员无法访问）。

定义泛型方法，只需将泛型参数列表置于返回值之前：

```java
public class GenericMethods {

    public static <E> void printClassName(E e) {
        System.out.printf(e.getClass().getSimpleName());
    }

    public static <E, T, F> void printSameMethods(E e, T t, F f) {
        Method[] ems = e.getClass().getMethods();
        Method[] tms = t.getClass().getMethods();
        Method[] fms = f.getClass().getMethods();

        Set<String> set = new HashSet<>(); // Set 集合元素不能重复
        for (Method m : ems) {
            set.add(m.getName());
        }
        for (Method m : tms) {
            set.add(m.getName());
        }
        for (Method m : fms) {
            set.add(m.getName());
        }

        for (String s : set) {
            System.out.printf(s + "\n");
        }

    }

    public static void main(String[] args) {
        // 不必指明参数类型
        GenericMethods.printClassName(AbstractCollection.class);
        System.out.printf("\n");
        GenericMethods.printSameMethods(HashSet.class, ArrayList.class, TreeMap.class);
    }

}
```

注意：当使用泛型类时，必须在创建对象的时候指定类型参数的值，而使用泛型方法的时候，通常不必指明参数类型，因为编译器会为我们找出具体的类型，这称为 *类型参数推断* 。

##### 泛型有时需要向程序中加入更多的代码

创建一个持有 List 的 Map：
在 JDK 1.6 之前，你需要通过如下方式声明：
`Map<Integer,List<String>> map = new HashMap<Integer, List<String>>();`
JDK 1.6 之后得到了简化：
`Map<Integer, List<String>> map = new HashMap<>();`

##### 显示的类型说明

在泛型方法中，可以显示的指明类型，但这种语法很少使用，要显示的指明类型，必须在点操作符与方法名之间插入尖括号，然后把类型置于尖括号内。

```java
public class Test {


    public static void main(String[] args) {

        Test test = new Test();
        test.f(test.<String, Integer>map());
    }

    public void f(Map map) {
    }

    public <K, V> Map<K, V> map() {
        return new HashMap<>();
    }

}

```

#### 4 可变参数与泛型

可变参数与泛型可以很好的共存。

```java
public class Test {


    public static void main(String[] args) {
        Test t = new Test();
        for (Integer i : t.list(1, 2, 3, 4, 5)) {
            System.out.printf(i + " ");
        }

    }

    public <T> List<T> list(T... ts) {
        return Arrays.asList(ts);
    }

}
```

输出

```bash
1 2 3 4 5 
```

#### 5 擦除的神秘之处

```java
    public static void main(String[] args) {
        Class c1 = new ArrayList<String>().getClass();
        Class c2 = new ArrayList<Integer>().getClass();
        System.out.printf(c1.equals(c2) ? "true" : "false");
    }
```

输出结果为 true，`ArrayList<String>` 和 `ArrayList<Integer>`很容易被认为是不同的类型，不同的类型在行为方面肯定不同，但事实是他们是同样的类型。

>*在泛型代码内部，无法获得任何有关泛型参数类型的信息*

java 是使用擦除来实现的，这意味着当你使用泛型时，任何具体的类型信息都被擦除了。

```java
public class Test {

    public static void main(String[] args) {
        Test t = new Test();
        t.test(t.new Te());
    }

    private <E> void test(E e) {
        e.f(); // 编译无法通过
    }

    class T {
        void f() {
        }
    }

    class Te extends T {
    }

}

```
上面的例子在泛型方法 test 中调用了 e 对象的 f 方法，对于 C++ ，这是允许的，C++ 编译器将对此进行检查，它看到 e 对象调用了 f 方法，而此时 e 映射到的类型 Te 拥有 f 方法，编译就通过，否则编译出错，这样就保证了类型安全。 java 则不同，由于有了擦除，编译器无法将 test 方法通过参数 e 调用 f 方法这一需求映射到 e 对象拥有 f 方法这一事实上。我们必须协助泛型类，给定泛型的边界，以此告知编译器只能接受遵循这个边界的类型。这样就能通过编译：

```java
public class Test {


    public static void main(String[] args) {
        Test t = new Test();
        t.test(t.new Te());
    }

    private <E extends T> void test(E e) {
        e.f(); // 编译通过
    }

    class T {
        void f() {
        }
    }

    class Te extends T {
    }

}

```

泛型类型擦除会擦除到他的第一个边界，因而 e 对象可以调用 T 类型的所有可访问方法。

#### 6 迁移兼容性

擦除是 java 泛型实现的一个折中，因为泛型不是 java 语言出现时就有的组成部分，所以这种折中是必须的。

泛型类型只有在静态类型检查期间才出现，在此之后，程序中的所有泛型类型都将被擦除，替换为他们的非泛型上界，例如，诸如 `List<T>`这样的类型擦除时将被替换为 List ，而普通的类型变量在未指定边界的情况下将被擦除为 `Object` 。

擦除的核心动机是使得泛化的客户端可以使用非泛化的类库，反之亦然，这经常被称为 迁移兼容性。java 泛型必须支持向后兼容性，即现有的代码和类文件仍旧合法，并且继续保持其之前的定义，并且当某个类库变为泛型时，不会破坏依赖于它的代码和应用程序。

#### 7 擦除的问题

擦除的主要正当理由是从非泛化代码到泛化代码的转变过程，以及在不破坏现有类库的情况下，将泛型融入 java 语言。

擦除的代价时显著的，泛型不能用于显示的引用运行时类型的操作中，例如转型、instanceof 和 new 操作，因为所有有关类型的信息都丢失了。

```java
public class Foo<T>{
    T var;
}
```

```java
Foo<String> f = new Foo<String>();
```

在泛型类 Foo 中，看似类型 T 都在被替换为 String ，但事实并非如此，无论何时，当你在编写这个类的代码时，必须提醒自己：“不，它只是一个 Object 。”

##### @SuppressWarnings("unchecked")

禁止【未检查类型】警告，该注解在 java SE 5 之前不支持。

这个注解被放置在产生这类警告的方法之上，而不是整个类上，当要关闭警告时，最好是尽量的“聚焦”，这样就不会因为过于宽泛的关闭警告，而导致意外的遮蔽掉正在的问题。

```java
public class Test {

    @SuppressWarnings("unchecked")
    public static void main(String[] args) {
        TC tc = new TC(); 
        tc.put(""); //未指明类型参数类型，因而在赋值时编译器无法进行类型安全检查

    }

    static class TC<T> {
        T t;

        void put(T t) {
            this.t = t;
        }

        T get() {
            return t;
        }
    }

}

```


#### 8 边界处的动作

```java
public class Test {

    @SuppressWarnings("unchecked")
    public static void main(String[] args) {
        TC tc = new TC();
        List list = tc.create(1, 2, 3, 4, 5); // 未检查警告
        System.out.println(list);
    }

    static class TC<T> {
        List<T> create(T... ts) {
            List<T> result = new ArrayList<T>();
            for (T t : ts) {
                result.add(t);
            }
            return result;
        }
    }
}
```

即使编译器无法知道有关 create 方法中的 T 的任何信息，但它仍旧可以确保你放置到 result 中的对象具有 T 类型，使其适合 `ArrayList<T>`，因此，即使擦除在方法或类内部移除了有关实际类型的信息，编译器仍旧可以在方法或类中使用类型的 **内部一致性** 。

因为擦除在方法体中移除了类型信息，所以在运行时的问题就是边界：即对象进入和离开方法的地点。这些正是编译器在编译期间执行类型检查并插入转型代码的地点。

为上述 TC 类添加如下方法：
` void set(T t) { ele = t; }`
当外部调用 set 方法时，编译器会进行类型检查，当然，前提是指定 TC 类的类型参数，即修改上述代码中 tc 的定义如下：
`TC<Integer> tc = new TC<>();`
这样，当向 set 方法传入错误的类型，编译器会提醒你（编译无法通过），若不指定类型参数，则只会得到未检查类型 警告。

添加如下方法：
`T get(){ return (T)"str";} // 此时会得到未检查类型转换异常` 
对比如下实现：
`T get(){ return ele;} // 合法`
外部调用：
`Integer num = tc.get(); // 指定类型参数为 Integer`
由于擦除，这里 get 方法中的 T 在运行时就是 Object 。但外部调用 get 方法时却无需进行显示的类型转换，然而这里的类型转换是必须的。事实是，编译器完成了这个动作，编译器会在编译期插入对传递出去值的转型。

修改后的代码：

```java
public class Test {

//    @SuppressWarnings("unchecked")
    public static void main(String[] args) {
        TC<Integer> tc = new TC<>();
//        tc.set(""); // 编译无法通过
        List list = tc.create(1, 2, 3, 4, 5);
        System.out.println(list);

        tc.set(13);
        Integer num = tc.get();
        System.out.println("\n" + num);
    }

    static class TC<T> {
        T ele;

        List<T> create(T... ts) { // 编译期类型检查
            List<T> result = new ArrayList<T>();
            for (T t : ts) {
                result.add(t);
            }
            return result;
        }

        // 边界就是发生动作的地方
        void set(T t) { // 编译期类型检查
            ele = t;
        }

        T get() {
            return ele; // 编译期转型代码插入发生的地方
        }
    }
}

```

#### 9 擦除的补偿

擦除丢失了在泛型代码中执行某些操作的能力，任何在运行时需要知道确切类型的操作都无法工作。

要想在运行时获得类型信息只能通过类型判断，当然，这就违背了 java 泛型的主旨：参数化类型

```java
public class Test {

    public static void main(String[] args) {
        TC<Person> tc = new TC<>();
        System.out.println(Person.class.getSimpleName());
        tc.check(new Person("tom", 12));
    }

    static class TC<T> {
        void check(T t) {
            if (t.getClass().equals(Person.class)) {
                Person p = (Person) t;
                System.out.println(p.getName());
            }
        }
    }

    static class Person {
        String name;
        int id;

        public Person(String name, int id) {
            this.name = name;
            this.id = id;
        }

        public String getName() {
            return name;
        }

        int getId() {
            return id;
        }
    }
}
```

需要知道确切类型的操作都无法工作。

```java
    static class TC<T> {
        void test() {
//            T[] ts = new T[1]; // 错误
//            T t = new T(); // 错误
//            T[] ts1 = (T[]) new Object[1]; // 未检查类型转换
        }
    }
```

##### 擦除补偿：类型标签

需要显示的传递类型的 Class 对象，以便可以在类型表达式中使用。

```java
public class Test {

    public static void main(String[] args) {
        TC<A> tc = new TC<>(A.class);
        System.out.println(tc.check(""));
        System.out.println(tc.check(new A()));
    }

    static class TC<T> {
        Class<T> kind;

        public TC(Class<T> kind) {
            this.kind = kind;
        }

        boolean check(Object obj) {
            return kind.isInstance(obj);
        }
    }

    static class A {
    }
}

```

输出：

```bash
false
true
```

`isInstance` 方法将检查 obj 的类型是否为 kind。

----------------

文章大部分摘抄自《Java 编程思想》 第 15 章 泛型，加上部分自己的理解，若有错误，欢迎指正。

------------
