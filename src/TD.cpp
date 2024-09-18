template<typename T>                //只对TD进行声明
class TD;                           //TD == "Type Displayer"


const char* const ptr =         //ptr是一个常量指针，指向常量对象 
    "Fun with pointers";
template<typename T>
void f(T param) {
    TD<T> x;
}

int main() {

    f(ptr);
    return 0;
}