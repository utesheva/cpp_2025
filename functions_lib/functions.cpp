#include "functions.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

TFunctionPtr FuncFactory::Create(const std::string& type, const std::vector<double>& params) {
    if (type == "ident") return std::make_shared<IdentityFunction>();
    if ((type == "const") && (params.size() == 1)) return std::make_shared<RealFunction>(params[0]);;
    if ((type == "const") && (params.size() != 1)) throw std::invalid_argument("Invalid number of parameters (need 1)");
    if ((type == "power") && (params.size() == 1)) return std::make_shared<PowerFunction>(params[0]);
    if ((type == "power") && (params.size() != 1)) throw std::invalid_argument("Invalid number of parameters (need 1)");
    if (type == "exp") return std::make_shared<Exponent>();
    if ((type == "polynomial") && (params.size() > 0)) return std::make_shared<Polynom>(params);
    if ((type == "polynomial") && (params.size() == 0)) throw std::invalid_argument("Invalid number of parameters (need 1)");
    throw std::invalid_argument("Invalid function name (use ident, const, power, exp or polynomial)");
}

double IdentityFunction::operator()(double x) const {
    return x;
}

double IdentityFunction::GetDerivative(double x) const {
    return 1;
}

std::string IdentityFunction::ToString() const {
    return "x";
}

RealFunction::RealFunction(double x) : constant(x) {}

double RealFunction::operator()(double x) const {
    return constant;
}

double RealFunction::GetDerivative(double x) const {
    return 0;
}

std::string RealFunction::ToString() const {
    return std::to_string(constant);
}

PowerFunction::PowerFunction(double x): power(x) {}

double PowerFunction::operator()(double x) const{
    return std::pow(x, power);
}

double PowerFunction::GetDerivative(double x) const {
    return power * std::pow(x, power - 1);
}

std::string PowerFunction::ToString() const {
    return "x^" + std::to_string(power);
}

double Exponent::operator()(double x) const{
    return std::exp(x);
}

double Exponent::GetDerivative(double x) const {
    return std::exp(x);
}

std::string Exponent::ToString() const {
    return "e^x";
}

Polynom::Polynom(const std::vector<double>& params): params(params) {}

double Polynom::operator()(double x) const {
    double res = 0;
    for (int i=0; i < params.size(); ++i) {
        res += std::pow(x, i) * params[i];
    }
    return res;
}

double Polynom::GetDerivative(double x) const {
    double res = 0;
    for (int i=1; i < params.size(); ++i) {
        res += i * std::pow(x, i - 1) * params[i];
    }
    return res;
}

std::string Polynom::ToString() const {
    std::string res = std::to_string(params[0]);
    for (int i=1; i < params.size(); ++i) {
        res += " + " + std::to_string(params[i]) + "x^" + std::to_string(i);
    }
    return res;
}
