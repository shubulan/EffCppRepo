#include <cstddef>
class UPNumber {
public:
    UPNumber();
    ~UPNumber();
private:
    static void * operator new(size_t size);
    static void operator delete(void *ptr);
};


int main() {
    UPNumber * a = new UPNumber;
    return 0;
}