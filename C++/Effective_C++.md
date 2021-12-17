### 一、让自己习惯C++

#### 条款01、视C++为一个语言联邦

​	c++高效程序则视状况而变化，取决于你使用c++的哪一部分

#### 条款02、尽量以 const 、enum、inline 替换 #define

​	“宁可以编译器替换预处理器”，因为或许 #define 不被视为语言的一部分

​	1、对于单纯变量，最好以 const 对象或 enums 替换 #define

​	2、对于形似函数的宏（macros），最好改用 inline 函数替换 #define

#### 条款03、尽可能使用 const

​	1、将某些东西声明为 const 可帮助编译器侦测出错误用法，const 可被施加于任何作用域内的对象、函数参数、函数返回类型、成员函数本体

​	2、编译器强制实施bitwise constness，但你编写程序时应该使用“概念上的常量性”（conceptual constness）

​	3、当const和non-const成员函数有着实质等价的实现时，令non-const版本调用const版本可避免代码重复

#### 条款04、确定对象被使用前已先被初始化

​	1、为内置型对象进行收工初始化，因为c++不保证初始化它们

​	2、构造函数最好使用成员初值列（member initialization list），而不要在构造函数本体内使用赋值操作。初值列列出的成员变量，其排列次序应该和它们在class中的声明次序相同

​	3、为免除“跨编译单元之初始化次序”问题，请以local static对象替换non-local static 对象

### 二、构造/析构/赋值运算

#### 条款05、了解C++默默编写并调用哪些函数

​	1、如果你打算在一个“内含reference成员”的class内支持赋值操作，你必须自己定义copy assignment操作符

​	2、面对“内含const成员”的class，编译器的反应跟上面是一样的，更改const成员是不合法的，所以编译器不知道如何在它自己生成的赋值函数内面对它

​	3、如果某个base class将copy assignment操作符声明为private，编译器将拒绝为其derived classe生成一个copy assignment

​	4、编译器可以暗自为class创建default构造函数、copy构造函数、copy assignm操作符、以及析构函数

#### 条款06、若不想使用编译器自动生成的函数，就该明确拒绝

​	1、为驳回编译器自动（暗自）提供的机能，可将相应的成员函数声明为private并且不予以实现

#### 条款07、为多态基类声明virtual析构函数

​	1、关于虚函数		
​	（1）vptr（virtual table pointer）指向一个由函数指针构成的数组，称为vtbl（virtual table）
​	（2）每一个带有virtual函数的class都有一个相应的vtbl
​	（3）当对象调用某一虚函数，实际被调用的函数取决于该对象的vtpr所指的那个vtbl----编译器在其中寻找适当的函数指针
​	（4）纯虚函数导致抽象类-----不能被实体化的class

​	2、起因
​		  一个 base class 指针指向一个生成的 derived class 对象，当derived class对象经由一个base class指针被删除，而该base class带着一个non-virtual析构函数，其结果未有定义-----实际执行时候通常发生的是对象的derived成分没被销毁

​	3、polymorphic（带多态性质）base classes应该声明一个virtual析构函数。如果class带有任何virtual函数，它就应该拥有一个virtual析构函数。

​	4、classes的设计目的如果不是为了作为base classes使用，或不是为了具备多态性，就不该声明virtual析构函数。

#### 条款08、别让异常逃离析构函数

​	1、问题提出：
​	（1）在一个类进行析构的时候，执行某个功能或者代码的时候，由可能会抛出异常，这个异常会导致不确定的结果，有可能影响到后面程序的运行
​	（2）C++不喜欢析构函数吐出异常
​	（3）如果你的析构函数必须执行一个动作，而该动作可能会在失败时抛出异常，要怎么办

​	2、解决方法----以一个class负责数据库连接为例子
​	（1）class DBConn 管理 class DBConnction
​	（2）把调用 close 的责任从 DBConn 析构函数手上移到 DBConn客户手上，但DBConn析构函数仍内含一个双保险调用
​	（3）由用户自己调用 close 并不会对他们带来负担，而是给他们一个处理错误的机会，否则他们没机会响应。

​	3、总结
​	（1）析构函数绝对不要吐出异常，如果一个被析构函数调用的函数可能抛出异常，析构函数应该捕捉任何异常，然后吞下他们（不传播）或结束程序
​	（2）如果客户需要对某个操作函数运行期间抛出的异常做出反应，那么class应该提供一个普通函数（而非在析构函数中）执行操作

#### 条款09、绝不在构造和析构过程中调用virtual函数

​	1、class 在构建的时候，先构建 base class，然后再构造 derived class。当 base class 构造函数执行时 derived class 的成员变量尚未初始化

​	2、在 base class 构造期间，virtual函数不是virtual函数。

​	3、对于析构函数，一旦 derived class 析构函数开始执行，对象内的 derived class 成员变量便呈现未定义值，也就是不存在的样子

​	4、base class 的构造函数里面调用的是 pure virtual 函数的话，编译无法通过

​	5、例子

​		  问：如何确保每次由某一父类继承体系上的对象被创建，就会有适当版本的成员函数被调用

​		  答：在父类中将该成员函数改为非虚，然后子类构造函数传递必要信息给父类的构造函数，而后那个构造函数便可安全调用该非虚函数

```c++
class base
{
public:
    explicit base(int& i){
        func(i);
    }
    void func(int& i);
};
class derived : public base
{
public:
    derived(parameters) : base(createPara(parameter)){}
private:
    static int createPara(parameter);
};
```

​		对于derived类中的 private static 函数 createPara的运用：	
​		（1）比起在成员初值列内给予父类所需数据，利用辅助函数创建一个值给父类构造函数往往比较方便和可读
​		（2）令该函数为static，也就不可能意外指向”初期未成熟之derived对象内尚未初始化的成员变量“
​		（3）正因为”那些成员变量处于未定义状态“，所以”在父类构造和析构期间调用的虚函数不可下降到派生类“

#### 条款10、令operator=返回一个reference to *this

​	1、为了实现“连锁赋值”，赋值操作符必须返回一个reference指向操作符的左侧实参

​	2、适用于所有赋值相关运算

#### 条款11、在operator=中处理“自我赋值”

​	1、确保当对象自我赋值时 operator= 有良好行为，其中技术包括比较“来源对象”和“目标对象”的地址、精心周到的语句顺序、以及copy-and-swap

​	2、确定任何函数如果操作一个以上的对象，而其中多个对象是同一个对象时，其行为仍然正确。

#### 条款12、复制对象时勿忘其每一个成分

