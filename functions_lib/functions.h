#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <memory>
#include <string>
#include <functional>

class TFunction {
public:
    virtual double operator()(double x) const = 0;
    virtual double GetDerivative(double x) const = 0;
    virtual std::string ToString() const = 0;
};

using TFunctionPtr = std::shared_ptr<TFunction>;

class FuncFactory {
public:
    TFunctionPtr Create (const std::string& type, const std::vector<double>& params = {});
};

class IdentityFunction: TFunction {
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

class RealFunction: TFunction {
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

class PowerFunction: TFunction {
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

class Exponent: TFunction {
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

class Polynom: TFunction {
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

#endif
