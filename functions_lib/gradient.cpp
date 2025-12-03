#include "gradient.h"

double GradientDesc(const TFunction& func, double initial, double learning_rate, int max_iter) {
    double x = initial;
    for (int i = 0; i < max_iter; ++i) {
        double value = func(x);
        double grad = func.GetDerivative(x);
        x = x - learning_rate * value / grad;
    }
    return x;
}
