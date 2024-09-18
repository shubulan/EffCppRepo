
```c++
/**
    为了给一个默认构造函数，提供了无意义的默认参数，导致创建出来的类可能无意义
 */

class EquipmentPiece {
public:
    EquipmentPiece(int IDNumber = UNSPECIFIED); // 永远不要这样做

private:
    int const int id;
};
```