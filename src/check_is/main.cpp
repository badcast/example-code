#include <iostream>
#include <utility>

#include <bits/stdc++.h>

using namespace std;

enum class ObjectType { ObjectUnknown, ObjectBool, ObjectInt, ObjectFloat };

const char* print_type(ObjectType obj)
{
    if (obj == ObjectType::ObjectInt) {
        return "Int";
    } else if (obj == ObjectType::ObjectBool) {
        return "Boolean";
    } else if (obj == ObjectType::ObjectFloat) {
        return "Float";
    }
    return "Unknown";
}

//template<typename T, typename V = void>
//ObjectType is_abc() {
//    return ObjectType::ObjectUnknown;
//}

template <typename T, typename std::enable_if<std::is_integral<T>::value, std::nullptr_t>::type = nullptr>
ObjectType is_abc() {
    return ObjectType::ObjectInt;
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, std::nullptr_t>::type = nullptr>
ObjectType is_abc() {
    return ObjectType::ObjectFloat;
}

template <typename T, typename std::enable_if<std::is_same<T, bool>::value, std::nullptr_t>::type = nullptr>
ObjectType is_abc(T) {
    return ObjectType::ObjectBool;
}

int main()
{
    cout << print_type(is_abc<int>()) << endl;
    cout << print_type(is_abc<float>()) << endl;
     //cout << print_type(is_abc<bool>::type) << endl;
    return 0;
}
