#include <iostream>
#include <cmath>

class TemperatureLaw {
public:
    virtual double get_next_temperature(int iter) const = 0;
};

class BoltzmannLaw : public TemperatureLaw {
private:
    double initial_temp;
public:
    BoltzmannLaw(double temp) : initial_temp(temp) {}
    double get_next_temperature(int iter) const override {
        return initial_temp / std::log(1 + iter);
    }
};

class CauchyLaw : public TemperatureLaw {
    private:
    double initial_temp;
public:
    CauchyLaw(double temp): initial_temp(temp) {}
    double get_next_temperature(int iter) const override {
        return initial_temp / (1 + iter);
    }
};

class LogarithmicCauchyLaw : public TemperatureLaw {
private:
    double initial_temp;
public:
    double get_next_temperature(int iter) const override {
        return initial_temp * (std::log(1 + iter) / (1 + iter));
    }
};
