#include "operators.h"
#include "functions.h"
#include <memory>


TFunctionPtr operator+(const TFunctionPtr& op1, const TFunctionPtr& op2) {
    Func func = [op1,op2](double x) { return (*op1)(x) + (*op2)(x); };
    Func deriv = [op1, op2](double x) { return op1->GetDerivative(x) + op2->GetDerivative(x); };
    std::string str = "(" + op1->ToString() + " + " + op2->ToString() + ")";
    return std::make_shared<Combination>(func, deriv, str);
}

TFunctionPtr operator-(const TFunctionPtr& op1, const TFunctionPtr& op2) {
    Func func = [op1, op2](double x) { return (*op1)(x) - (*op2)(x); };
    Func deriv = [op1, op2](double x) { return op1->GetDerivative(x) - op2->GetDerivative(x); };
    std::string str = "(" + op1->ToString() + " - " + op2->ToString() + ")";
    return std::make_shared<Combination>(func, deriv, str);
}

TFunctionPtr operator*(const TFunctionPtr& op1, const TFunctionPtr& op2) {
    Func func = [op1, op2](double x) { return (*op1)(x) * (*op2)(x); };
    Func deriv = [op1, op2](double x) {
        return op1->GetDerivative(x) * (*op2)(x) + (*op1)(x) * op2->GetDerivative(x);
    };
    std::string str = "(" + op1->ToString() + " * " + op2->ToString() + ")";
    return std::make_shared<Combination>(func, deriv, str);
}

TFunctionPtr operator/(const TFunctionPtr& op1, const TFunctionPtr& op2) {
    Func func = [op1, op2](double x) { return (*op1)(x) / (*op2)(x); };
    Func deriv = [op1, op2](double x) {
        double f1 = (*op1)(x);
        double f2 = (*op2)(x);
        double df1 = op1->GetDerivative(x);
        double df2 = op2->GetDerivative(x);
        return (df1 * f2 - f1 * df2) / (f2 * f2);
    };
    std::string str = "(" + op1->ToString() + " / " + op2->ToString() + ")";
    return std::make_shared<Combination>(func, deriv, str);
}
