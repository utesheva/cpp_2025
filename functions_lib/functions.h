#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <memory>
#include <string>
#include <functional>
#include <vector>

class TFunction {
public:
    virtual double operator()(double x) const = 0;
    virtual double GetDerivative(double x) const = 0;
    virtual std::string ToString() const = 0;
};

using TFunctionPtr = std::shared_ptr<TFunction>;

class FuncFactory {
public:
    TFunctionPtr Create(const std::string& type, const std::vector<double>& params);
};

class IdentityFunction: public TFunction {
public:
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

class RealFunction: public TFunction {
private:
    double constant;
public:
    explicit RealFunction(double x);
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

class PowerFunction: public TFunction {
private:
    double power;
public:
    explicit PowerFunction(double x);
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

class Exponent: public TFunction {
public:
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

class Polynom: public TFunction {
private:
    std::vector<double> params;
public:
    explicit Polynom(const std::vector<double>& params);
    double operator()(double x) const override;
    double GetDerivative(double x) const override;
    std::string ToString() const override;
};

#endif
