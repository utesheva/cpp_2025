#include "functions.h"
#include "operators.h"
#include "gradient.h"
#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>

const double EPSILON = 1e-6;

// Test 1 (basic functions abd derivative)

class FuncFactoryTest : public ::testing::Test {
protected:
    FuncFactory factory;
};

TEST_F(FuncFactoryTest, CreateIdentityFunction) {
    auto f = factory.Create("ident", {});
    EXPECT_DOUBLE_EQ((*f)(1), 1.0);
    EXPECT_DOUBLE_EQ((*f)(-3.5), -3.5);
    EXPECT_DOUBLE_EQ(f->GetDerivative(10), 1.0);
    EXPECT_EQ(f->ToString(), "x");
}

TEST_F(FuncFactoryTest, CreateConstantFunction) {
    auto f = factory.Create("const", {42.0});
    EXPECT_DOUBLE_EQ((*f)(1), 42.0);
    EXPECT_DOUBLE_EQ((*f)(-3.5), 42.0);
    EXPECT_DOUBLE_EQ(f->GetDerivative(10.0), 0.0);
    EXPECT_EQ(f->ToString(), "42");
}

TEST_F(FuncFactoryTest, CreatePowerFunction) {
    // x^2
    auto f2 = factory.Create("power", {2});
    EXPECT_DOUBLE_EQ((*f2)(0.0), 0.0);
    EXPECT_DOUBLE_EQ((*f2)(3.0), 9.0);
    EXPECT_DOUBLE_EQ((*f2)(-2.0), 4.0);
    EXPECT_NEAR(f2->GetDerivative(5.0), 10.0, EPSILON);
    // x^3
    auto f3 = factory.Create("power", {3.0});
    EXPECT_DOUBLE_EQ((*f3)(2.0), 8.0);
    EXPECT_DOUBLE_EQ((*f3)(-2.0), -8.0);
    EXPECT_NEAR(f3->GetDerivative(2.0), 12.0, EPSILON);
    // x^0.5
    auto fsqrt = factory.Create("power", {0.5});
    EXPECT_NEAR((*fsqrt)(4.0), 2.0, EPSILON);
    EXPECT_NEAR((*fsqrt)(9.0), 3.0, EPSILON);
    EXPECT_NEAR(fsqrt->GetDerivative(4.0), 0.25, EPSILON);
}

TEST_F(FuncFactoryTest, CreateExponentFunction) {
    auto f = factory.Create("exp", {});
    EXPECT_DOUBLE_EQ((*f)(0.0), 1.0);
    EXPECT_NEAR((*f)(1.0), std::exp(1.0), EPSILON);
    EXPECT_NEAR((*f)(2.0), std::exp(2.0), EPSILON);
    EXPECT_NEAR((*f)(-1.0), std::exp(-1.0), EPSILON);
    EXPECT_NEAR(f->GetDerivative(0.0), 1.0, EPSILON);
    EXPECT_NEAR(f->GetDerivative(1.0), std::exp(1.0), EPSILON);
    EXPECT_EQ(f->ToString(), "e^x");
}

TEST_F(FuncFactoryTest, CreatePolynomial) {
    // 1 + 2x + 3x^2
    auto f = factory.Create("polynomial", {1.0, 2.0, 3.0});
    EXPECT_DOUBLE_EQ((*f)(0.0), 1.0);
    EXPECT_DOUBLE_EQ((*f)(1.0), 6.0);   // 1 + 2 + 3
    EXPECT_DOUBLE_EQ((*f)(2.0), 17.0);  // 1 + 4 + 12
    EXPECT_DOUBLE_EQ((*f)(-1.0), 2.0);  // 1 - 2 + 3
    // Производная: 2 + 6x
    EXPECT_NEAR(f->GetDerivative(0.0), 2.0, EPSILON);
    EXPECT_NEAR(f->GetDerivative(1.0), 8.0, EPSILON);
}

TEST_F(FuncFactoryTest, ThrowsOnInvalidParameters) {
    EXPECT_THROW(factory.Create("const", {}), std::invalid_argument);
    EXPECT_THROW(factory.Create("const", {1.0, 2.0}), std::invalid_argument);
    EXPECT_THROW(factory.Create("power"), std::invalid_argument);
    EXPECT_THROW(factory.Create("polynomial"), std::invalid_argument);
    EXPECT_THROW(factory.Create("unknown"), std::invalid_argument);
    EXPECT_THROW(factory.Create("sin"), std::invalid_argument);
    EXPECT_THROW(factory.Create("", {}), std::invalid_argument);
}

// Test 2 (arithmetics)
class OperatorsTest : public ::testing::Test {
protected:
    FuncFactory factory;
    TFunctionPtr x;      // ident
    TFunctionPtr c5;     // const 5
    TFunctionPtr x2;     // x^2
    TFunctionPtr polynom;     // 1 + 2x + 3x^2
    TFunctionPtr ex;     // e^x
    void SetUp() override {
        x = factory.Create("ident");
        c5 = factory.Create("const", {5});
        x2 = factory.Create("power", {2});
        polynom = factory.Create("polynomial", {1, 2, 3});
        ex = factory.Create("exp", {});
    }
};

TEST_F(OperatorsTest, Addition) {
    // x + 5
    auto f1 = x + c5;
    EXPECT_DOUBLE_EQ((*f1)(0.0), 5.0);
    EXPECT_NEAR(f1->GetDerivative(10.0), 1.0, EPSILON);
    // x^2 + 1 + 2x + 3x^2
    auto f2 = x2 + polynom;
    EXPECT_DOUBLE_EQ((*f2)(2.0), 21.0);  // 4 + 1 + 4 + 12
    EXPECT_NEAR(f2->GetDerivative(2.0), 18.0, EPSILON); // 2*2 + 2 + 6*2
}

TEST_F(OperatorsTest, Subtraction) {
    // 1 + 2x + 3x^2 - x^2
    auto f1 = polynom - x2;
    EXPECT_DOUBLE_EQ((*f1)(2.0), 13.0);   // 1 + 2*2 + 3*4 - 4
    EXPECT_NEAR(f1->GetDerivative(2.0), 10.0, EPSILON); // 2 + 6*2 - 2*2
    // 5 - x
    auto f2 = c5 - x;
    EXPECT_DOUBLE_EQ((*f2)(3.0), 2.0);
    EXPECT_NEAR(f2->GetDerivative(10.0), -1.0, EPSILON);
}

TEST_F(OperatorsTest, Multiplication) {
    // x * x = x^2
    auto f1 = x * x;
    EXPECT_DOUBLE_EQ((*f1)(0.0), 0.0);
    EXPECT_DOUBLE_EQ((*f1)(3.0), 9.0);
    EXPECT_DOUBLE_EQ((*f1)(5.0), 25.0);
    EXPECT_NEAR(f1->GetDerivative(4.0), 8.0, EPSILON); // 2x
    // x^2 * 5
    auto f2 = x2 * c5;
    EXPECT_DOUBLE_EQ((*f2)(2.0), 20.0);
    EXPECT_NEAR(f2->GetDerivative(3.0), 30.0, EPSILON); // 5 * 2x
    // x * e^x
    auto f3 = x * ex;
    EXPECT_NEAR((*f3)(1.0), std::exp(1.0), EPSILON);
    // Производная: 1*e^x + x*e^x = e^x(1+x)
    EXPECT_NEAR(f3->GetDerivative(0.0), 1.0, EPSILON);
    EXPECT_NEAR(f3->GetDerivative(1.0), 2*std::exp(1.0), EPSILON);
}

TEST_F(OperatorsTest, Division) {
    // x^2 / x = x
    auto f1 = x2 / x;
    EXPECT_DOUBLE_EQ((*f1)(2.0), 2.0);
    EXPECT_DOUBLE_EQ((*f1)(5.0), 5.0);
    EXPECT_NEAR(f1->GetDerivative(3.0), 1.0, EPSILON);
    // x / x^2 = 1/x
    auto f2 = x / x2;
    EXPECT_DOUBLE_EQ((*f2)(2.0), 0.5);
    EXPECT_DOUBLE_EQ((*f2)(4.0), 0.25);
    // Производная: -1/x^2
    EXPECT_NEAR(f2->GetDerivative(2.0), -0.25, EPSILON);
    // 5 / x
    auto f3 = c5 / x;
    EXPECT_DOUBLE_EQ((*f3)(5.0), 1.0);
    EXPECT_NEAR(f3->GetDerivative(5.0), -0.2, EPSILON); // -5/x^2
}

TEST_F(OperatorsTest, CompositeExpressions) {
    // (x + 5) * x = x^2 + 5x
    auto sum = x + c5;
    auto f1 = sum * x;
    EXPECT_DOUBLE_EQ((*f1)(3.0), 24.0);  // 9 + 15
    EXPECT_NEAR(f1->GetDerivative(2.0), 9.0, EPSILON);
    // (x^2 + x) / x = x + 1
    auto sum2 = x2 + x;
    auto f2 = sum2 / x;
    EXPECT_DOUBLE_EQ((*f2)(5.0), 6.0);
    EXPECT_NEAR(f2->GetDerivative(10.0), 1.0, EPSILON);
    // x * (x + 5) * x = x^3 + 5x^2
    auto prod = x * sum;
    auto f3 = prod * x;
    EXPECT_DOUBLE_EQ((*f3)(2.0), 28.0);  // 8 + 20
    EXPECT_NEAR(f3->GetDerivative(2.0), 32.0, EPSILON);
    // (x^2 + e^x) * (x - 5)
    auto diff = x - c5;
    auto f4 = (x2 + ex) * diff;
    EXPECT_NEAR((*f4)(6.0), 36.0 + std::exp(6.0), EPSILON);
    EXPECT_NEAR(f4->GetDerivative(2.0), -2 * std::exp(2.0) - 8, EPSILON);
}

TEST_F(OperatorsTest, ThrowsOnInvalidParameters) {
    EXPECT_THROW(x + "abc", std::logic_error);
    EXPECT_THROW(5 - x, std::logic_error);
    EXPECT_THROW(c5 * 5, std::logic_error);
    EXPECT_THROW(5 / c5, std::logic_error);
    EXPECT_THROW(x2 + 5, std::logic_error);
    EXPECT_THROW("qwerty" - x2, std::logic_error);
    EXPECT_THROW(polynom * 'a', std::logic_error);
}

// Test ToString
TEST(ToStringTest, BasicFunctions) {
    FuncFactory factory;
    auto x = factory.Create("ident", {});
    EXPECT_EQ(x->ToString(), "x");
    auto ex = factory.Create("exp", {});
    EXPECT_EQ(ex->ToString(), "e^x");
    auto sum = x + ex;
    EXPECT_EQ(sum->ToString(), "(x + e^x)");
    auto prod = x * ex;
    EXPECT_EQ(prod->ToString(), "(x * e^x)");
}


// Test Gradient Descent
class GradientDescentTest : public ::testing::Test {
protected:
    FuncFactory factory;
};

TEST_F(GradientDescentTest, FindMinimumOfQuadratic) {
    // f(x) = (x-2)^2 = x^2 - 4x + 4, минимум в x=2
    auto f = factory.Create("polynomial", {4.0, -4.0, 1.0});
    double result1 = GradientDesc(*f, 0.0, 0.1, 1000);
    EXPECT_NEAR(result1, 2.0, EPSILON);
    double result2 = GradientDesc(*f, 5.0, 0.1, 1000);
    EXPECT_NEAR(result2, 2.0, EPSILON);
}

TEST_F(GradientDescentTest, FindMinimumOfCubic) {
    // f(x) = x^3 - 3x^2, производная: 3x^2 - 6x
    auto f = factory.Create("polynomial", {0.0, 0.0, -3.0, 1.0});
    double result = GradientDesc(*f, 1.0, 0.01, 2000);
    EXPECT_NEAR(result, 2.0, EPSILON);
}

TEST_F(GradientDescentTest, FindMinimumOfExponentialPlusQuadratic) {
    // f(x) = e^x + (x-1)^2
    auto exp_func = factory.Create("exp", {});
    auto quad = factory.Create("polynomial", {1.0, -2.0, 1.0}); // (x-1)^2 = x^2 - 2x + 1
    auto f = exp_func + quad;
    double result = GradientDesc(*f, 0.0, 0.000000001, 1000);
    EXPECT_NEAR(result, 0.0, EPSILON);
}

TEST_F(GradientDescentTest, ConstantFunction) {
    // f(x) = 5, производная = 0
    auto f = factory.Create("const", {5.0});
    double initial = 10.0;
    double result = GradientDesc(*f, initial, 0.1, 1000);
    EXPECT_DOUBLE_EQ(result, initial);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
