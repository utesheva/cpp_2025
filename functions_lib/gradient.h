#ifndef GRADIENTDESC_H
#define GRADIENTDESC_H
#include "functions.h"

double GradientDesc(const TFunction& func, double initial = 0, double learning_rate = 0.01, int max_iter=1000);

#endif
