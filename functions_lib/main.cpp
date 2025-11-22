#include "functions.h"
#include "operators.h"
#include <iostream>

int main() {
std::vector<TFunctionPtr> cont = {};
auto f = FuncFactory().Create("power", {2}); // PowerFunc x^2
cont.push_back(f);
auto g = FuncFactory().Create("polynomial", {7, 0, 3, 15}); // TPolynomial 7 + 3*x^2 + 15*x^3
cont.push_back(g);
for (const auto ptr : cont) {
std::cout << ptr->ToString() << " for x = 10 is " << (*ptr)(10) << std::endl;
}
auto p = *f + *g;
std::cout << p->GetDerivative(1) << std::endl; // 53
auto h = *f + "abc"; // std::logic_error
std::cout << f->GetDerivative(3) << std::endl; // 429
}
