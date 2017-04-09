#include <iostream>
union UnionType {
    // int
    int8_t int1;
    int16_t int2;
    int32_t int4;
    int64_t int8;

    // unsigned int
    uint8_t uint1;
    uint16_t uint2;
    uint32_t uint4;
    uint64_t uint8;

    // float point
    float f;
    double d;

    // ptr
    void *ptr;
};
enum class TypeName {
    Int1,
    Int2,
    Int4,
    Int8,

    Uint1,
    Uint2,
    Uint4,
    Uint8,

    Float,
    Double,

    Pointer,
    String
};
struct Var {
    TypeName type;
    UnionType value;
    size_t length;
    Var(){
        length = 0;
        value.uint8 = 0;
    }
};