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

## 

