#ifndef OPERATORS_H
#define OPERATORS_H
#include "functions.h"
#include <stdexcept>
#include <type_traits>

TFunctionPtr operator+(const TFunctionPtr& op1, const TFunctionPtr& op2);
TFunctionPtr operator-(const TFunctionPtr& op1, const TFunctionPtr& op2);
TFunctionPtr operator*(const TFunctionPtr& op1, const TFunctionPtr& op2);
TFunctionPtr operator/(const TFunctionPtr& op1, const TFunctionPtr& op2);

template<typename T>
typename std::enable_if<!std::is_base_of<TFunction, T>::value, TFunctionPtr>::type
operator+(const TFunctionPtr& op1, const T& op2) {
    throw std::logic_error("Invalid operands in addition");
}

template<typename T>
typename std::enable_if<!std::is_base_of<TFunction, T>::value, TFunctionPtr>::type
operator+(const T& op1, const TFunctionPtr& op2) {
    throw std::logic_error("Invalid operands in addition");
}

template<typename T>
typename std::enable_if<!std::is_base_of<TFunction, T>::value, TFunctionPtr>::type
operator-(const TFunctionPtr& op1, const T& op2) {
    throw std::logic_error("Invalid operands in substraction");
}

template<typename T>
typename std::enable_if<!std::is_base_of<TFunction, T>::value, TFunctionPtr>::type
operator-(const T& op1, const TFunctionPtr& op2) {
    throw std::logic_error("Invalid operands in substraction");
}

template<typename T>
typename std::enable_if<!std::is_base_of<TFunction, T>::value, TFunctionPtr>::type
operator*(const TFunctionPtr& op1, const T& op2) {
    throw std::logic_error("Invalid operands in multiplication");
}

template<typename T>
typename std::enable_if<!std::is_base_of<TFunction, T>::value, TFunctionPtr>::type
operator*(const T& op1, const TFunctionPtr& op2) {
    throw std::logic_error("Invalid operands in multiplication");
}

template<typename T>
typename std::enable_if<!std::is_base_of<TFunction, T>::value, TFunctionPtr>::type
operator/(const TFunctionPtr& op1, const T& op2) {
    throw std::logic_error("Invalid operands in division");
}

template<typename T>
typename std::enable_if<!std::is_base_of<TFunction, T>::value, TFunctionPtr>::type
operator/(const T& op1, const TFunctionPtr& op2) {
    throw std::logic_error("Invalid operands in division");
}
#endif
