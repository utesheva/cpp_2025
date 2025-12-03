#include "functions.h"
#include "operators.h"
#include <iostream>
#include "gradient.h"

int main() {
FuncFactory factory;

TFunctionPtr p1 = factory.Create("polynomial", {2, 1});
TFunctionPtr p2 = factory.Create("polynomial", {-3, 1});
TFunctionPtr p = p1 * p2;
// double GradientDesc(const TFunction& func, double initial, double learning_rate, int max_iter)
std::cout << p->ToString() << std::endl;
std::cout << GradientDesc(*p, 12, 0.01, 1000) << std::endl;
return 0;
}
