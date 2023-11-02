#pragma once

#include <type_traits>

// ignore unsigned type

template<class integer> struct sign_type{ using type = integer; };
template<> struct sign_type<unsigned char> { using type = char; };
template<> struct sign_type<unsigned int> { using type = int; };
template<> struct sign_type<unsigned short> { using type = short; };
template<> struct sign_type<unsigned long long> { using type = long long; };

template<class T>
struct _basic_type_check {
    constexpr static bool value = false;
};

template<>
struct _basic_type_check<char>
{
    constexpr static bool value = true;
};
template<>
struct _basic_type_check<short>
{
    constexpr static bool value = true;
};
template<>
struct _basic_type_check<int>
{
    constexpr static bool value = true;
};
template<>
struct _basic_type_check<long long>
{
    constexpr static bool value = true;
};
template<>
struct _basic_type_check<bool>
{
    constexpr static bool value = true;
};
template<>
struct _basic_type_check<float>
{
    constexpr static bool value = true;
};
template<>
struct _basic_type_check<double>
{
    constexpr static bool value = true;
};


template<class T>
inline constexpr bool is_basic_type = _basic_type_check<typename sign_type<typename std::remove_cv<T>::type>::type>::value;


