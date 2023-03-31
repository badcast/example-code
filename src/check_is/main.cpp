#include <iostream>
#include <type_traits>

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

#if __cplusplus == 201103L

template<typename T>
typename std::enable_if<std::is_same<T, bool>::value, ObjectType>::type
get_type() {
    return ObjectType::ObjectBool;
}

template<typename T>
typename std::enable_if<!std::is_same<T, bool>::value && std::is_integral<T>::value, ObjectType>::type
get_type() {
    return ObjectType::ObjectInt;
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, ObjectType>::type
get_type() {
    return ObjectType::ObjectFloat;
}

template<typename T>
typename std::enable_if<!std::is_same<T, bool>::value && !std::is_integral<T>::value && !std::is_floating_point<T>::value, ObjectType>::type
get_type() {
    return ObjectType::ObjectUnknown;
}

#else

template<typename T>
ObjectType get_type() {
    if constexpr (std::is_same_v<T, bool>) {
        return ObjectType::ObjectBool;
    } else if constexpr (std::is_integral_v<T>) {
        return ObjectType::ObjectInt;
    } else if constexpr (std::is_floating_point_v<T>) {
        return ObjectType::ObjectFloat;
    } else {
        return ObjectType::ObjectUnknown;
    }
}

#endif

int main()
{
    cout << print_type(get_type<bool>()) << endl;
    cout << print_type(get_type<int>()) << endl;
    cout << print_type(get_type<float>()) << endl;
    cout << print_type(get_type<char>()) << endl;
    return 0;
}

