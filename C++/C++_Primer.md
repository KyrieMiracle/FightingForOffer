## 第2章 变量和基本类型

### 1、void* 指针 p50

（1）void*是一种特殊的指针类型，可用于存放任意对象的地址。

（2）我们对该地址中到底是个什么类型的对象并不了解

（3）操作空间有限：拿它和别的指针比较、作为函数的输入输出，或者赋给另外一个void*指针。不能直接操作它所指的对象，因为我们不知道这个对象到底是什么类型，也就无法确定能在这个对象上做哪些操作

（4）以void*的视角来看内存空间也就仅仅是内存空间，没办法访问内存空间中所存的对象



### 2、const 限定符 p53

#### 2.1基本概念

（1）希望定义一种变量，它的值不能被改变，可以随时警惕防止程序一不小心改变了这个值

（2）如果想在多个文件之间共享const对象，必须在变量的定义之前添加extern关键字

#### 2.2 const 的引用

（1）与普通引用不同的是，对常量的引用不能被用作修改它所绑定的对象

（2）当初始化常量引用的类型与被引用对象的类型不一样的时候，编译器会创建一个临时变量，让它被绑定，这种引用改变值是非法的

#### 2.3 const 指针

（1）指向常量的指针，不能用于改变其所指对象的值。要想存放常量对象的地址，只能使用指向常量的指针

（2）所谓指向常量的指针或引用，不过是指针或引用“自以为是”罢了，它们觉得自己指向了常量，所以自觉地不去改变所指对象的值

（3）常量指针必须初始化

（4）把*放在const关键字之前用以说明指针是一个常量，这样的书写形式隐含着一层意味，即不变的是指针本身的值而非指向的那个值

```c++
int a  = 0;
int * const p1 = &a;  // p1 将一直指向 a
const double b = 0;
const double * const p2; // p2 是一个指向常量对象的常量指针

/* 弄清定义最有效的方法是：从右向左阅读定义 */
```

#### 2.4 顶层const

（1）顶层const 表示指针本身是个常量，底层const 表示指针所指的对象是一个常量

（2）当执行对象的拷贝操作时，拷入和拷出的对象必须具有相同的底层const资格，或者两个对象的数据雷西那个必须能够转换，一般来说，非常量可以转换成常量，反之则不行

#### 2.5 constexpr 和 常量表达式

（1）常量表达式是指值不会改变并且在编译过程就能得到计算结果的表达式

（2）将变量声明为 constexpr 类型以便由编译器来验证变量的值是否是一个常量表达式

（3）声明为 constexpr 的变量一定是一个常量，而且必须用常量表达式初始化

（4）在 constexpr 声明中如果定义了一个指针，限定符 constexpr仅对指针有效，与指针所指的对象无关



### 3、decltype类型指示符 p62

（1）希望从表达式的类型推断出要定义的变量的类型，但不想用该表达式的值初始化变量

（2）作用：选择并返回操作数的数据类型，编译器分析表达式并得到它的类型，并不实际计算表达式的值