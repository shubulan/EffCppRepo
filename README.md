# 总结 Effective C++ 系列图书

## 1. 一个类该不该有默认构造函数？
[EquipmentPiece](./src/equipmentpiece.md)
思考：如果类没有默认构造函数，会遇到哪些问题？
* 无法建立数组
* 有些模板容器类需要默认构造函数
* 派生类必须显示初始化基类，也就必须知道基类构造函数参数的含义。

思考：如果为了派生类使用方便，而强行提供默认构造函数，会怎样？
类中的一些数据成员会处在不确定的一个状态。为了处理这些不确定状态，而使整个类变复杂了。


## 2. 如何思考模板类型推断？
在模板参数中，我们关心的类型其实有两个，1. param 是什么类型 2. T 是什么类型。我们建立以下下模型来说明这个问题
模型：
```c++
template<typename T>
void f(ParamType param);

f(expr);
```
ParamType 与 T 通常是不一样的，因此二者都需要推断。

步骤一：根据 expr 和 ParamType 推断 param 的类型，可以按照直觉来。
步骤二：根据 param 类型与 ParamType 进行模式匹配来决定 T

实际场景中，ParamType 有三种情况

1. ParamType 是指针或者引用，但不是通用引用

案例一：
```c++
template<typename T>
void f(T& param);               //param是一个引用

int x=27;                       //x是int
// 凭直觉，param 的类型应该是 int&
f(x);    // 因此，T是int

const int cx=x;                 //cx是const int
// 凭直觉，param 的类型应该是 const int&
f(cx);  // 因此 T 是 const int

const int& rx=x;                //rx是指向作为const int的x的引用
// 凭直觉，param 的类型应该是 const int&
f(rx);  // 因此 T 是 const int
```

案例二：
```c++
template<typename T>
void f(const T& param);         //param现在是reference-to-const

int x=27;                       //x是int
// 凭直觉，param 的类型应该是 const int&
f(x);    // 因此，T 是 int

const int cx=x;                 //cx是const int
// 凭直觉，param 的类型应该是 const int&
f(cx);  // 因此 T 是 int

const int& rx=x;                //rx是指向作为const int的x的引用
// 凭直觉，param 的类型应该是 const int&
f(rx);  // 因此 T 是 int
```

2. ParamType 是通用引用
在这里，凭直觉来推断 param 类型不太好使了。需要记住一个规则：如果 expr 是左值，那么 T 和 ParamType 都是左值引用。如果 expr 是右值，那么方式和 [1.] 一样
```c++
template<typename T>
void f(T&& param);              //param现在是一个通用引用类型
		
int x=27;                       //x是int
// x 是左值 param 的类型应该是 int&
f(x);    // T 是 int&

const int cx=x;                 //cx是const int
// cx 是左值，param 的类型应该是 const int&
f(cx);  // T 是 const int&

const int& rx=x;                //rx是指向作为const int的x的引用
// rx 是左值，param 的类型应该是 const int&
f(rx);  // T 是 const int&

// 27 是右值，param 的类型应该是 int&&
f(27);  // T 是 int
```
3. ParamType 既不是引用，也不是指针

这里都看作传值
```c++
template<typename T>
void f(T param);                //以传值的方式处理param


int x=27;                       //x是int
// param 的类型应该是 int
f(x);    // T 是 int

const int cx=x;                 //cx是const int
//param 的类型应该是 int
f(cx);  // T 是 int

const int& rx=x;                //rx是指向作为const int的x的引用
// param 的类型应该是 int
f(rx);  // T 是 int


const char* const ptr =         //ptr是一个常量指针，指向常量对象 
    "Fun with pointers";

// param 的类型应该是 const char* 
f(ptr);   // T 的类型是 const char*
```

## 3. 如何查看编译器推导结果？

这个例子我们不确定，不妨用这种方式试一下，见 `src/TD.cpp`
```c++
const char* const ptr =         //ptr是一个常量指针，指向常量对象 
    "Fun with pointers";
// param 的类型应该是 const char*，真的吗？
f(ptr);   // T 的类型是 const char*
```
编译输出结果是
```
src/TD.cpp: In instantiation of ‘void f(T) [with T = const char*]’:
src/TD.cpp:14:6:   required from here
src/TD.cpp:9:11: error: ‘TD<const char*> x’ has incomplete type
    9 |     TD<T> x;
```
我们将立刻知道，T 的类型就是 `const char*`

## 如何确保对象只能在堆上分配？

在栈上创建的对象有一个特点，就是一定会自动调用它的析构函数。
```c++
void func() {
    Type a;
    // a 的析构函数被执行
}

```
如果把析构函数定义为私有的，那么该类将不能在栈上创建，否则会编译报错。代码见:`src/heapOnly.cpp`,编译报错为
```
src/heapOnly.cpp:11:14: error: ‘UPNumber::~UPNumber()’ is private within this context
   11 |     UPNumber a;
      |              ^
src/heapOnly.cpp:6:5: note: declared private here
    6 |     ~UPNumber();
      |     ^
```

但是如果考虑继承的情况，那将无法保证该类的对象一定在堆上。因为子类可以在栈上，那么子类的基类部分也在栈上。

没有可移植的办法来判断一个对象在栈上还是堆上。

## 如何禁止在堆上创建对象？
只需要把 `operator new` 和 `operator delete` 定义成私有的即可，见 `src/heapForbidden.cpp`


## 如何实现引用计数
引用计数通常基于共享值的假设。如果这个假设不成立，引用计数通常只会带来更多的复杂性而已。


## 区分 RAII 和引用计数
RAII(Resource Acquisition Is Initialization) 与引用计数通常被搞混。因为c++ 11 引入的智能指针，使用了 RAII 技术。RAII 通常用于自动处理资源的释放，是一种管理资源、避免泄漏的技术。引用计数能够自动处理资源的释放，但引用计数的出发点，并不是处理资源的释放问题。引用计数有两个动机：
1. 所有相同的对象共享一个值实现
2. 简化跟踪堆上对象的过程。一个对象被 new 出来，最要紧的事是跟踪该对象的所有权。因为其所有者需要负责对该资源释放。
引用计数只是实现 RAII 的技术之一


举两个例子。对于动机1，


## 三五法则
三五法则的落脚点是析构函数。如果一个类需要析构函数，那么它通常需要：copy 构造，赋值，移动构造，移动赋值，析构函数共计五个。