#include <cstring>
#include <iostream>
using std::cout;
using std::endl;
using std::ostream;

class String {
public:
  String(const char *str = "") : len(strlen(str)), data(new char[len + 1]) {
    memcpy(data, str, len);
    data[len] = '\0';
  }

  String(const String &rhs) {
    len = rhs.len;
    data = new char[len + 1];
    memcpy(data, rhs.data, len);
    data[len] = '\0';
  }
  String(String &&rhs) noexcept {
    len = rhs.len;
    data = rhs.data;
    rhs = String(); // 共享空串
  }
  String &operator=(String &&rhs) {
    if (this == &rhs)
      return *this;
    delete[] data;
    len = rhs.len;
    data = rhs.data;
    rhs = String(); // 共享空串
    return *this;
  }

  String &operator=(const String &rhs) {
    if (this == &rhs)
      return *this;

    delete[] data;
    len = rhs.len;
    data = new char[len + 1];
    memcpy(data, rhs.data, len);
    data[len] = '\0';
    return *this;
  }
  ostream &print(ostream &os) const {
    os << data << endl;
    return os;
  }

  ~String() { delete[] data; }

private:
  size_t len;
  char *data;
};

ostream &operator<<(ostream &os, const String &val) {
  val.print(os);
  return os;
}

int main() {
  String s("hello"); // construct
  String a, b, c, d; // default construct
  String e = s;      // copy construct
  a = b = c = d = s; // asignment
  cout << s << e;
  cout << a << b << c << d;

  return 0;
}