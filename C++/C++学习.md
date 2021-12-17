# C++学习

## 一、基础知识

### 1、函数指针与函数的引用

#### 1.1 概念

```c++
void hello(int a)
{
	cout<<"hello, value:"<<a<<endl;
}

int main()
{  
    typedef void(*hellofunc)(int);  //更改别名
    hellofunc func_1 = hello;
    func_1(8)
    cin.get();
}
```

#### 2.2 用处

函数指针作为函数的参数进行传递，即在设计一个函数时，里面需要调用一个函数，但那个函数是个参数，作为一个接口

```c++
void PrintValue(int value)
{
    cout<<"value:"<<value<<endl;
}
void ForEach(const vector<int>& values, void(*func)(int))
{
    for(int value : values)
    {
        func(value);
    }
}
int main()
{
    vector<int> values = {1,2,5,4};
    ForEach(values, PrintValue);
    return 0;
}
```

#### 2.3 Lambda表达式

本质上是我们定义一种叫做匿名函数的方式，一次性，不用另外创建一个函数

用法是，在我们会设置函数指针指向函数的任何地方，我们都可以将它设置为lambda

```c++
void ForEach(const vector<int>& values, void(*func)(int))
{
    for(int value : values)
    {
        func(value);
    }
}
int main()
{
    vector<int> values = {1,2,5,4};
    ForEach(values, [](int value){ cout<<"value:"<<value<<endl; });// []中填写传递的方式
    
    auto it = find_if(values.begin(), value.end(), [](){ return value >3 });
    
    return 0;
}
```



fun & name()



### 2、this指针的运用

this指针指向被调用的成员函数所属的对象

用途：当形参和成员变量同名时，可用this指针来区分

​		   在类的非静态成员函数中返回对象本身，可使用return *this

```c++
class Person
{
    public:
    	int m_age;
    public:
    	Person(int age)
        {
            m_age = age;
        }
    	Person& PersonAddAge(Person &p) //函数引用，引用方式返回
        {
            this->m_age += p.m_age;
            return *this;
        }
};

void test01()
{
    Person p1(10);
    Person p2(20);
    p2.PersonAddAge(p1).PersonAddAge(p1); //链式编程思想
    cout<<"p2的年龄"<<p2.m_age;
}
```



### 3、拷贝构造函数

默认拷贝构造函数

```c++
Person(const Person & p)
{
    m_Age = p.m_Age;
}
```

浅拷贝与深拷贝的讨论



### 4、符号重载

#### 4.1 加号运算(+)符重载

```c++
//1、通过成员函数重载+
Person Person:: operator+ (Person &p)
{
    Person temp;
    temp.m_A = this->m_A + p.m_A;
    temp.m_B = this->m_B + p.m_B;
    return temp;
}
//下面两个语句等价：
Person p3 = p1.operator+(p2);
Person p3 = p1 + p2;

//2、通过全局函数重载+
Person operator+ (Person &p1, Person &p2)
{
    Person temp;
    temp.m_A = p1.m_A + p2.m_A;
    temp.m_B = p1.m_B + p2.m_B;
    return temp;
}
//下面两个语句等价：
Person p3 = operator+(p1, p2);
Person p3 = p1 + p2;

```

#### 4.2 左移运算符(<<)重载

目的是可以输出整个对象等

不能通过成员函数进行重载

```c++
ostream& operator<<(ostream& cout, Person& p)  
{
    cout<<"m_A = "<<p.m_A<<" m_B = "<<p.m_B;
    return cout;
}

cout<<p<<endl;   //使用引用函数，则可以链式编程
```

#### 4.3 递增运算符(++)重载

通过成员函数进行重载

```c++
class MyInteger
{
    private:
    	int m_Num;
    public:
    	MyInteger(int a):m_Num(a){};
    //1、重载前置++运算符：
    	MyInteger& operator++()
        {
            m_Num++;
            return *this;
        }
    //2、重载后置++运算符：
    	MyInteger operator++(int)  //int代表占位参数，可以用于区分前置和后置递增
        {
            //先 记录当前结果
            MyInteger temp = *this;
            //后 递增
            m_Num++;
            return temp;
        }
};
```

注：后置++不能使用函数引用

#### 4.4 赋值运算符(=)重载

避免浅拷贝所带来的内存重复释放（进行析构的时候）的问题，与前面拷贝函数的深浅拷贝问题相联系

意义：系统的=提供的浅拷贝操作，对=进行重载可以避免浅拷贝所带来的问题

```c++
class Person
{
    private:
    	int *m_Age;
    public:
    	Person(int age)  //构造函数
        {
            m_Age = new int(age);
        }
    	~Person()        //析构函数
        {
            if(m_Age != NULL)
            {
                delete m_Age;
                m_Age = NULL;
            }
        }
    
    	Person& operator=(Person &p)  //函数引用是为了能够可以实现 p3 = p2 = p1
        {
            //编译器是提供浅拷贝，即
            //m_Age = p.m_Age;
            //但我们应该先判断是否有属性在堆区，如果有先释放赶紧，然后进行深拷贝
            if(m_Age != NULL)
            {
                delete m_Age;
                m_Age = NULL;
            }
            this->m_Age = new int(*p.m_Age);
            return *this;
        }
    	
};
```

#### 4.5 关系运算符(==)重载

其中包括==,<,>,!=，在成员函数中实现如下：

```c++
bool operator==(Person &p)
{
    if(this->m_Age == p.m_Age && this->m_Name == p.m_Name)
    {
        return true;
    }
    else return false;
}
```

#### 4.6 函数调用运算符重载

由于重载后使用的方式非常像函数的调用，因此称为仿函数

仿函数使用灵活，没有固定写法，在后面stl中常用

```c++
class MyPrint
{
    public:
    	void operator()(string text)
        {
            cout<<text<<endl;
        }
};
void test01()
{
    MyPrint myPrint;
    myPrint("hello");  
}
```



### 5、继承

#### 5.1 对象模型

··父类中所有非静态成员属性都会被子类继承下来

··父类中私有成员属性是被编译器给隐藏了，因此是访问不到，但是确实是被继承下来了

··先构造父类，再构造子类，析构的顺序与构造的顺序相反

#### 5.2 同名成员的处理

非静态成员：

··直接访问，是访问子类的成员，  访问父类的成员，则加作用域

·· 如果子类中出现和父类同名的成员函数，子类的同名成员会隐藏掉父类中所有同名成员函数，所以如果要访问父类中被隐藏的同名成员函数，需要加作用域

静态成员：

```c++
//1、通过对象访问
Son s;
s.m_A;
s.Base::m_A;
//2、通过类名访问
Son::m_A;
Son::Base::m_A;
//第一个::代表通过类名方式访问，第二个代表访问父类作用域下
```

#### 5.3 菱形继承

概念：

​	两个派生类继承同一个基类

​	又有某个类同时继承上面两个派生类

问题：

​	两个父类有相同的东西都同时继承了

```c++
class Animal
{
    public:
    	int m_Age;
};
//利用虚继承，解决菱形继承的问题
//在继承之前加上关键字 virtual, 最大的类为虚基类
class Sheep :virtual public Animal{};
class Tuo :virtual public Animal{};
class SheepTuo :public Sheep, public Tuo{};

void test()
{
    SheepTuo st;
    //st.m_Age, st.Sheep::m_Age ,st.Tuo::m_Age 是同一个东西
}
```

底层实现的时候会出现虚基指针(vbptr)



### 6、多态

#### 6.1 基本概念

分类：

··静态多态：函数重载 和 运算符重载，复用函数名

··动态多态：派生类和虚函数实现运行时多态

两者的区别：

··静态多态的函数地址早绑定 -- 编译阶段确定函数地址

··动态多态的函数地址晚绑定 -- 运行阶段确定函数地址

```c++
class Animal
{
	public:
    	virtual void speak()  //虚函数
        //虚函数则实现动态多态
        //如果不加virtual，则会地址早绑定
        {
            cout<<"animal is speaking"<<endl;
        }
}；
class Cat :public Animal
{
    public:
    	virtual void speak()
        {
            cout<<"cat is speaking"<<endl;
        }
};

//形式1
void doSpeak01(Animal &animal)
{
    animal.speak()；
}
void test01()
{
	Cat cat;
    doSpeak01(cat);  //Animal &animal = cat, 父类与子类间的转换
                    //如果上面没有加virtual的话，则会输出"animal"
}

//形式2
void test02()  //常用这种方法
{
    Animal * animal = new Cat;  //的对象不同，输出的东西不同
    animal->speak(); //输出的是cat is speaking
}

//形式3
void doSpeak02(Animal & animal)
{
    animal->speak();
    delete animal;
}
void test03()
{
    doSpeak(new Cat);
}
```

动态多态的满足条件：

​	1、有继承关系
​	2、子类重写父类的虚函数

动态多态的使用：

​	父类的指针或者引用，执行子类对象

原理：

​	虚函数是起底层出现 vfptr 和 vftable

好处：

​	组织结构清晰、可读性强、对于前期和后期拓展以及维护性高 

#### 6.2 纯虚函数和抽象类

​	在多态中，通常父类中虚函数的实现是毫无意义的，主要都是调用子类重写的内容
​	因此可以将虚函数改成纯虚函数

​	纯虚函数语法：virtual 返回值类型 函数名 （参数列表）=0

​	当类中有了纯虚函数，这个类被称为抽象类，其特点如下：
​	1、无法实例化对象
​	2、子类必须重写抽象类中的纯虚函数，否则也属于抽象类

#### 6.3 虚析构和纯虚构

​	多态使用时，如果子类中有属性开辟到堆区，那么父类指针在释放时无法调用子类的析构代码

​	解决方法：将父类中的析构函数改为 虚析构 或 纯虚析构

​	虚析构和纯虚析构的共性：
​	1、可以解决父类指针释放子类对象
​	2、都需要有具体的函数实现，但实现方式不一样，纯虚析构要在类外进行实现

​	两者的区别：如果是纯虚析构，该类属于抽象类，无法实例化对象

​	语法与纯虚函数类似：virtual ~类名() = 0;



### 7、一些关键字

#### 7.1 mutable

字面意思是可以改变的，可以对比const

标记类成员为mutable，意味着，类中的const方法可以修改这个成员

另一个用处是在lambda表达式

```c++
class Test
{
private:
    string m_Name;
    mutable int m_Count = 0;
public:
    const string& GetName() const
    {
        m_Count++;
        return m_Name;
    }
};

int main()
{
    const Test a;
    a.GetName();
    
    int x;
    auto func = [=]() mutable
    {
        x++;
        cout<<x<<endl;
    };
    func();
    
}
```



#### 7.2 const

概念：相当于一种承诺，即保证该数据量是不可以修改的

1、在指针中的用法

```c++
const int* a = new int; //不能修改 a 所指的元素的值
int const* a = new int; //跟上面是一样的
int* const a = new int; //不能改变 a 所表示的地址
```

2、在类中的用法

```c++
class Test
{
private:
	int m_X, m_Y;
    mutable int val;
public:
    int GetX const
    {
        // const承诺了不能修改成员函数
        // m_X = 1; 这是错误的
        val = 2; //这个变量在声明的时候是用了mutable，因此可以绕过const的作用
        return m_X
    }
};

void Print(const Test& T) //建议传参是引用方式或者指针方式传递，这样可以避免形参传递需要复制而导致时间浪费
{
    cout<<T.GetX<<endl; //加了const后传参数进来，只能调用const的成员函数
}
```



#### 7.3 new和malloc的区别

[https://www.cnblogs.com/ywliao/articles/8116622.html]()



#### 7.4 static

1、静态成员函数，即static menber functions没有this指针，它只能处理静态成员变量

2、调用静态函数的方式：通过对象来调用；通过类名称来调用（不用创建对象）

#### 7.5 关于数组与指针的问题

```c++
int * a[10];  // a是含有10个整形指针的数组
int (*b)[10]; // b是指向一个含有10个整数
person * p = new person[10]; // p[i]里面存的是person类型，不是指针
```



#### 7.6 含有可变形参的函数

书P197 initalizer_list 形参

如果所有的实参类型相同，可以传递一个名为 initializer_list 的标准库类型



#### 7.7 void*指针

1、void类型指针可以指向任意类型的数据，即可用任意数据类型的指针对void指针赋值

```c++
int *pint;
void *pvoid;
pvoid = pint;
```

2、如果要将 pvoid 赋给其他类型指针，则需要强制类型转换

```c++
pint = (int*)pvoid
```

3、void指针不能解引用

4、如果函数的参数可以是任意类型的指针，那么应声明其参数为 void*
      典型的如内存操作函数 memcpy 和 memset 的函数原型：

```c++
void* memcpy(void* dest, const void* src, size_t len);
void* memset(void* buffer, intc, size_tnum);
```

​      这样，任何类型的指针都可以传入memcpy和memset中，这也真实体现了内存操作函数的意义，因为它操作的对象仅仅是一片内存，而不论这片内存是什么类型

参考干货：

https://zhuanlan.zhihu.com/p/163676489



## 二、提高编程

### 1、泛型编程--模板

#### 1.1 函数模板

```c++
template<typename T>  //声明一个模板，T是一个通用数据类型，typename可用class替换
//以交换数据为例子
void mySwitch(T &a, T&b)
{
    T temp = a;
    a = b;
    b = temp;
}
void test()
{
    int a = 10;
    int b = 20;
    //方法一：自动类型推导
    mySwitch(a,b);
    //方法二：显示指定类型
    mySwitch<int>(a,b);
}
```

​	注意事项：
​	1、自动类型推导，必须推导出一致的数据类型 T ，才可以使用
​	2、模板必须要确定出 T 的数据类型，才可以使用 

​	普通函数 与 函数模板 区别：
​	1、普通函数调用 和 函数模板的显示指定类型 可以发生隐式类型转换
​	2、函数模板的自动类型推导 不可以发生隐式类型转换

​	普通函数 和 函数模板 调用规则：
​	1、如果函数模板和普通函数都可以调用，优先调用普通函数
​	2、可以通过空模板参数列表，强制调用函数模板 

```c++
func<>(elem)  //<>为空模板参数列表 
```

​	3、函数模板可以发生函数重载
​	4、如果函数模板可以产生更好的匹配，优先调用函数模板

​	模板的局限性：
​		有些特定数据类型，如 类，需要用具体化方式做特殊实现

```c++
template<typename T>
bool Compare(T &a, T&b){....}
template<> bool Compare(Person &p1, Person &p2){....} //具体化实现，优先调用
```



#### 1.2 类模板

​	**基本语法例子**：

```c++
template<class NameType, class AgeType>
//默认参数写法：
template<class NameType, class AgeType = int>  
    
class Person
{
public:
    Person(NameType name, AgeType age);
    void showPerson(){};
    NameType m_Name;
    AgeType m_Age;
};
//类模板成员函数的类外实现
template<class T1, class T2>
Person<T1, T2>::Person(T1 Name, T2 Age)
{
	this->m_Name = name;
	this->m_Age = age;
}

//类模板对象做函数参数说明：
//1、指定传入类型
void printPerson1(Person<string, int> &p){};

//2、参数模板化 
template<class NameType, class AgeType>
void printPerson2(Person<NameType, AgeType> &p)
{
    cout<<"NameType 的类型为："<<typeid(NameType).name()<<endl;
    cout<<"AgeType  的类型为："<<typeid(AgeType).name()<<endl;
};

//3、整个类模板化
template<class T>
void printPerson2(T &p)
{
    p.showPerson();
}

/*............*/
void test01()
{
    Person<string, int> p1("K", 23); //无法用自动类型推导 
    printPerson1(p1);  //指定传入类型--->最常用
    printPerson2(p1);  //参数模板化
    printPerson3(p1);  //整个类模板化
}
```

​	**类模板 与 函数模板 得主要区别：**
​	1、类模板没有自动类型推导的使用方式
​	2、类模板在模板参数列表中可以有默认参数

​	**类模板中的成员函数创建时机：**
​	1、普通类中的成员函数一开始就可以创建
​	2、类模板中的成员函数在调用时才创建

```c++
class Person1
{
public:
	void show1();
};
class Person2
{
public:
	void show2();
};
template<class T>
class MyClass
{
public:
	T.obj;
    void func1()  //编译时能通过的，因为在未调用前，成员函数未创建
    {
        obj.show1;
    }
    void func2()
    {
        obj.show2;
    }
};
void test01()
{
    Myclass<Person1> p1;
    p1.show1;
    //p1.show2 是错误的
}
```

​	**类模板对象做函数的参数：**
​	1、指定传入类型------>最常用
​	2、参数模板化
​	3、整个类模板化

​	**类模板与继承：**
​	1、当子类继承的父类是一个类模板时，子类在声明的时候，要指定父类中的类型
​	2、如果不指定，编译器无法给子类分配内存
​	3、如果想灵活指定出父类中的 T 的类型，子类也需变为类模板

```c++
template<class T>
class Base
{
    T m;
};
template<class T1, class T2>
class Son:public Base<T2>
{
    T1 obj;
};
```

​	**类模板成员函数的类外实现：**

​	**类模板分文件编写问题以及解决**
​	方法一：直接包含源文件
​	方法二：将.h和.cpp中的内容写到一起，将后缀名改为.hpp文件----->常用

​	**类模板与友元：**
​	全局函数类内实现：直接在类内声明友元即可
​	全局函数类外实现：需要提前让编译器知道全局函数的存在

​	**类模板案例----数组封装**
​	P184  P185  P186



### 2、STL

#### 2.1  基本概念

​	**六大基本组件**
​	1、容器：各种数据结构，如vector、list、deque、set、map等，用来存放数据
​	2、算法：各种常用算法，如sort、find、copy、for_each等
​	3、迭代器：扮演了容器与算法之间的胶合剂
​	4、仿函数：行为类似函数，可作为算法的某种策略
​	5、适配器：一种用来修饰容器或者仿函数或迭代器接口的东西
​	6、空间配置器：负责空间的配置与管理



#### 2.2 初识

初识vector

```c++
#include <vector>
#include <algorithm>  //标准算法头文件

void myPrint(int val)
{
    cout<<val<<endl;
}

void test01()
{
    //创建一个vector容器，数组
    vector<int> v;
    //向容器中插入数据
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.push_back(40);
    //通过迭代器访问容器中的数据
    vector<int>::iterator itBegin = v.begin(); //起始迭代器 指向容器中第一个元素
    vector<int>::iterator itEnd = v.end();     //结束迭代器 指向容器中最后一个元素的下一个位置
    //第一种遍历方式
    while(itBegin != itEnd)
    {
        cout<<*itBegin<<endl;
        itBegin++;
    }
    
    //第二种遍历方式
    for(vector<int>::iterator it = v.begin(); it!=v.end();it++)
    {
        cout<<*it<<endl;
    }
    
    //第三章遍历方式
    for_each(v.begin(),v.end(),myPrint);
}
```

存放自定义数据类型

```c++
class Person
{
  public:
    Person(string name, int age):m_Name(name),m_Age(age){}
    string m_Name;
    int m_Age;
};

void test01()
{
    vector<Person *> v;
    Person p1("1",10);
    Person p2("2",20);
    Person p3("3",30);
    
    v.push_back(p1);
    v.push_back(p2);
    v.push_back(p3);
    
    //遍历容器中的数据
    for(vector<Person>::iterator it = v.begin(); it != v.end(); it++)
    {
        cout<<"姓名："<<it->m_Name<<"年龄:"<<it->m_Age<<endl;
    }
}

//存放自定义类型的指针
void test02()
{
    vector<Person> v;
    Person p1("1",10);
    Person p2("2",20);
    Person p3("3",30);
    
    v.push_back(&p1);
    v.push_back(&p2);
    v.push_back(&p3);
    
    for(vector<Person *>::iterator it = v.begin(); it != v.end(); it++)
    {
        cout<<"姓名："<<(*it)->m_Name<<"年龄:"<<(*it)->m_Age<<endl;
    }
}
```

vector容器嵌套容器

```c++
void test01()
{
    vector< vector<int> > v;
    //创建小容器
    vector<int> v1;
    vector<int> v2;
    vector<int> v3;
    //向小容器中添加数据
    for(int i = 0; i<3; i++)
    {
        v1.push_back(i+1);
        v2.push_back(i+2);
        v3.push_back(i+3);
    }
    //将小容器插入到大容器中
    v.push_back(v1);
    v.push_back(v2);
    v.push_back(v3);
    //通过大容器，把所有数据遍历一遍
    for(vector<vector<int>>::iterator it = v.begin(); it != v.end(); it++)
    {
        //(*it)是一个容器vector<int>
        for(vector<int>::iterator vit = (*it).begin(); vit != (*it).end(); vit ++)
        {
            cout<<*vit<<" ";
        }
        cout<<endl;
    }
}
```





#### 2.3 String

​	string是一个类，类内部封装了char*，管理这个字符串

##### 2.3.1 string 构造函数

```c++
string();                  //创建一个空的字符串，例如：string str;
string(const char* s);     //使用字符串s初始化
string(const string& str); //使用一个string对象初始化另一个string对象----拷贝构造
string(int n, char c);     //使用n个字符c初始化
```

##### 2.3.2 string 赋值操作

```c++
string& operator=(const char* s);     //char*类型字符串 赋值给当前的字符串
string& operator=(const string &s);   //把字符串s赋给当前字符串
string& operator=(char c);            //字符赋值给当前的字符串
string& assign(const chat *s);        //把字符串s赋给当前的字符串
string& assign(const chat *s, int n); //把字符串s的前n个字符赋给当前的字符串
string& assign(const string &s);      //把字符串s赋给当前字符串
string& assign(int n, char c);        //用n个字符c赋给当前字符串
```

##### 2.3.3 string 字符串拼接

```c++
string& operator+=(const char* str);            //重载+=操作符
string& operator+=(const char c);               //重载+=操作符
string& operator+=(const string& str);          //重载+=操作符
string& append(const char *s);                  //把字符串s连接到当前字符串结尾
string& append(const char *s,int n);            //把字符串s的前n个字符连接到当前字符串结尾
string& append(const string &s);                //同operator+=(const string& str)
string& append(const string &s, int pos, int n) //字符串s中从pos开始的n个字符串连接到字符串结尾
```

##### 2.3.4 string 字符串查找和替换

```c++
int find(const string& str, int pos=0) const;       //查找str第一次出现位置，从pos开始查找
int find(const char* s, int pos=0) const;           //查找s第一次出现位置，从pos开始查找
int find(const char* s, int pos, int n) const;      //从pos位置查找s的前n个字符第一次位置
int find(const char c, int pos = 0) const;          //查找字符c第一次出现位置
int rfind(const string& str, int pos = npos) const; //查找str最后一次出现位置，从pos开始查找
int rfind(const char* s, int pos = npos) const;     //查找s最后一次出现位置，从pos开始查找
int rfind(const char* s, int pos, int n) const;     //从pos查找s的前n个字符最后一次位置
int rfind(const char c, int pos = 0) const;         //查找字符c最后一次出现位置
string& replace(int pos, int n, const string& str); //替换从pos开始n个字符为字符串str
string& replace(int pos, int n, const char* s);     //替换从pos开始的n个字符为字符串s
```

​	rfind 与 find 的区别：rfind从右往左找，find从左往右找

##### 2.3.5 string 字符串比较

```c++
int compare(const string &s) const;   //与字符串s比较
int compare(const char *s) const;     //与字符串s比较
```

= 返回 0；> 返回 1；< 返回 -1；

##### 2.3.6 string 字符串存取

```c++
char& operator[](int n); //通过[]方式取字符
char& at(int n);         //通过at方式获取字符
```

​	访问后可对其进行修改

##### 2.3.7 string 字符串插入和删除

```c++
string& insert(int pos, const char* s);      //插入字符串
string& insert(int pos, const string& str);  //插入字符串
string& insert(int pos, int n, char c);      //在指定位置插入n个字符c
string& erase(int pos, int n = npos);        //删除从pos开始的n个字符
```

##### 2.3.8 string 子串获取

```c++
string substr(int pos = 0, int n = npos) const; //返回由pos开始的n个字符组成的字符串
```

​	一般是先进行查找得到pos，再进行截取

##### 2.3.9 处理string对象中的字符

| cctype头文件中的函数 |                                                    |
| :------------------: | -------------------------------------------------- |
|       isalnum        | 当c是字母或数字时为真                              |
|       isalpha        | 当c是字母时为真                                    |
|       iscntrl        | 当c是控制字符时为真                                |
|       isdigit        | 当c是数字时为真                                    |
|       isgraph        | 当c不是空格但可打印时为真                          |
|       islower        | 当c是小写字母时为真                                |
|       isprint        | 当c是可打印字符时为真（即c是空格或具有可视形式）   |
|       ispunct        | 当c是标点符号时为真                                |
|       isspace        | 当c是空白时为真                                    |
|       isupper        | 当c是大写字母时为真                                |
|       isxdigit       | 当c是十六进制数字时为真                            |
|       tolower        | 如果c是大写字母，输出对应的小写字母，否则原样输出c |
|       toupper        | 如果c是小写字母，输出对应的大写字母，否则原样输出c |



#### 2.4 Vector

​	动态扩展：并不是在原空间之后续接新空间，而是找更大的内存空间，然后将原数据拷贝新空间，释放原空间

​	vector容器的迭代器是支持随机访问的迭代器

##### 2.4.1 Vector 构造函数

```c++
vector<T> v;                 //采用模板实现 类实现，默认构造函数 后面用v.push_back(elem)
vector(v.begin(), v.end());  //将v[ begin(), end() )区间中的元素拷贝给本身
vector(n, elem);             //构造函数将n个elem拷贝给本身
vector(const vector &vec);   //拷贝构造函数
```

##### 2.4.2 Vector 赋值操作

```c++
vector& operator=(const vector &vec); //重载等号运算符
assign(beg, end);                     //将[ beg, end )区间中的数据拷贝赋值给本身
assign(n, elem);                      //将n个elem拷贝赋值给本身
```

##### 2.4.3 Vector 容量和大小

```c++
empty();               //判断容器是否为空
capacity();            //容器的容量
size();                //返回容器中元素的个数
resize(int num);       //重新指定容器的长度为num，若容器边长，则默认值填充新位置
					   //若容器变短，则末尾超出容器长度的元素被删除
resize(int num, elem)  //重新指定容器的长度为num，若容器边长，则以elem值填充新位置
    				   //若容器变短，则末尾超出容器长度的元素被删除
```

##### 2.4.4 Vector 插入和删除

```c++
push_back(elem);                                 //尾部插入元素elem
pop_back();                                      //删除最后一个元素
insert(const_iterator pos, ele);                 //迭代器指向位置pos插入元素elem
insert(const_iterator pos, int count, elem);     //迭代器指向位置pos插入count个元素elem
erase(const_iterator pos);                       //删除迭代器指向的元素
erase(const_iterator start, const_iterator end); //删除迭代器从start到end之间的元素
clear();                                         //删除容器中所有元素
```

##### 2.4.5 Vector 数据存取

```c++
at(int idx); //返回索引idx所指的数据
operator[];  //返回索引idx所指的数据
front();     //返回容器中的第一个数据元素
back();      //返回容器中的最后一个数据元素
```

##### 2.4.6 Vector 互换容器

```c++
swap(vec); //将vec与本身的元素互换
```

​	实际用途：巧用 swap 可以收缩内存空间

```c++
void test01()
{
    vector<int> v;
    for(int i = 0; i<1000; i++)
    {
        v.push_back(i)
    }            
    //此时v的容量为1300多
   //      大小为1000
    
    v.resize(3) 
    //此时v的容量仍然为1300多
   //      大小变为3
    vector<int>(v).swap(v);
    //此时v的容量和大小都为3
}
// vector<int>(v)为匿名对象，是通过拷贝构造函数进行创建的
```

##### 2.4.7 Vector 预留空间

​	减少vector在动态扩展容量时的扩展次数

```c++
reserve(int len); //容器预留len个元素长度，预留位置不初始化，元素不可访问
```

#### 2.5 Deque

​	双端数组，可以对头端进行插入删除操作

​	deque 与 vector 的区别：

​		1、vector 对于头部的插入删除效率低，数据量越大，效率越低
​		2、deque 相对而言，对头部的插入删除速度会比 vector 快
​		3、vector 访问元素时的速度会比 deque 快，这和两者内部实现有关

​	deque 内部工作原理：

​		内部有个中控器，维护每段缓冲区中的内容，缓冲区中存放真实数据
 	   中控器维护的是每个缓冲区的地址，使其使用 deque 时像一片连续的内存空间

##### 2.5.1 Deque 构造函数

```c++
deque<T> deqT;           //默认构造形式
deque(beg, end);         //构造函数将[beg,end)区间元素拷贝给本身
deque(n, elem);          //构造函数将n个elem拷贝给本身
deque(const deque &deq); //拷贝构造函数
```

##### 2.5.2 Deque 赋值操作

```c++
deque& operator=(const deque &deq); //重载等号操作符
assign(beg, end);                   //将[beg, end)区间中的数据拷贝赋值给本身
assign(n, elem);                    //将n个elem拷贝赋值给本身
```

##### 2.5.3 Deque 大小操作

```c++
empty();               //判断容器是否为空
size();                //返回容器中元素的个数
resize(int num);       //重新指定容器的长度为num，若容器边长，则默认值填充新位置
					   //若容器变短，则末尾超出容器长度的元素被删除
resize(int num, elem)  //重新指定容器的长度为num，若容器边长，则以elem值填充新位置
    				   //若容器变短，则末尾超出容器长度的元 素被删除
```

​	没有容量

##### 2.5.4 Deque 插入和删除

```c++
//插入
push_back(elem);                                 //尾部插入元素elem
push_front(elem);                                //头部
pop_back();                                      //删除最后一个元素
pop_front();                                     //第一个
//删除
insert(const_iterator pos, ele);                 //迭代器指向位置pos插入元素elem
insert(const_iterator pos, int count, elem);     //迭代器指向位置pos插入count个元素elem
insert(pos, beg, end);
erase(const_iterator pos);                       //删除迭代器指向的元素
erase(const_iterator start, const_iterator end); //删除迭代器从start到end之间的元素
clear();                                         //删除容器中所有元素
```

##### 2.5.5 Deque 数据存取

```c++
at(int idx); //返回索引idx所指的数据
operator[];  //返回索引idx所指的数据
front();     //返回容器中第一个数据元素
back();      //返回容器中最后一个数据元素
```

##### 2.5.6 Deque 排序

```c++
sort(iterator begin, iterator end); //对begin和end区间内元素进行排序
```

##### 2.5.7 priority_queue 优先队列

（1）本质上是一个堆实现

（2）使用：

```c++
#include <queue>
priority_queue<Type, Container, Functional> q;
// Type --- 数据类型
// Container --- 容器类型，必须是用数组实现得容器，如vector、deque，stl默认是vector
// Functional --- 比较方式，默认是最大堆，一般传入仿函数

//自定义数据类型下的实现最大堆
//方法一：运算符”<“重载
struct tmp{
    int x;
    tmp(int a) x(a){};
    bool operator<(const tmp& a) const{
        return x < a.x
    }
};
//方法二：重写仿函数
class mycomparison{
public:
    bool operator()(const T& a, const T& b){
        return a.x < b.x   //b可以理解为已经在堆里面的元素
    }
}
```

（3）题目：347 前K个高频元素



#### 2.6 Stack

​	先进后出

​	栈不允许有遍历行为，可以判断容器是否为空 empty，可以返回容器大小

```c++
//构造函数
stack<T> stk;                          //默认构造
stack(const stack &stk);               //拷贝构造
//赋值操作
stack& operator=(const stack &stk);    //重载等号操作符
//数据存取：
push(elem);   //向栈顶添加元素
pop();        //从栈顶移除第一个元素
top();        //返回栈顶元素
//大小操作
empty();      //判断堆栈是否为空
size();       //返回栈的大小
```



#### 2.7 Queue

​	先进先出

```c++
//构造函数
queue<T> que;                          //默认构造
queue(const queue &que);               //拷贝构造
//赋值操作
queue& operator=(const queue &que);    //重载等号操作符
//数据存取：
push(elem);   //向栈顶添加元素
pop();        //从栈顶移除第一个元素
back();
front();
//大小操作
empty();      //判断堆栈是否为空
size();       //返回栈的大小
```



#### 2.8 List

#### 2.9 函数对象

1、概念
（1）重载函数调用操作符的类，其对象常称为函数对象
（2）函数对象使用重载的()时，行为类似函数调用，也叫仿函数
（3）本质：函数对象（仿函数）是一个类，不是一个函数

2、使用
（1）函数对象在使用时，可以像普通函数那样调用，可以有参数，可以有返回值
（2）函数对象超出普通函数概念，函数对象可以有自己的状态
（3）函数对象可以作为参数传递

3、谓词
（1）返回bool类型的仿函数成为谓词
（2）如果operator()接收一个参数，那么叫做一元谓词
（3）如果operator()接收两个参数，那么叫做二元谓词
（4）用在函数传参

```c++
class mycompare{
public:
    bool operator()(int a, int b){
        return a > b;
    }
};
int main(){
    vector<int> nums = {1,5,6,8,7,4};
    sort(nums.begin(), nums.end(), mycompare());
}

```







## 三、书本学习内容

### 1、动态内存与智能指针

P405 

为了实现我们所希望的数据共享，我们为每个StrBlob设置一个shared_ptr来管理动态分配的vector





## 四、补充知识

### 1、关于参数传递和参数返回

#### 1.1 参数传递

​	（1）传值 pass by value

​	（2）传引用 pass by reference

#### 1.2 参数返回

​	（1）返回引用

​			---> 用引用作函数的返回值的，最大的好处是在内存中不产生返回值的副本
​			---> 当函数返回一个引用时，则返回一个指向返回值的隐式指针

​			常见应用场景：拷贝构造函数

