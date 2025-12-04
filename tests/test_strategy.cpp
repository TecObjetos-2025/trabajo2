#include <gtest/gtest.h>
#include "patterns/DescuentoPorcentaje.h"
#include "patterns/DescuentoFijo.h"
#include "patterns/DescuentoNulo.h"

/**
 * @file test_strategy.cpp
 * @brief Pruebas unitarias para las estrategias de descuento utilizando Google Test.
 * @author Fabricio Alonso Balarezo Delgado, Juan Manuel Cari Quispe, Anthony Ronaldo Cahui Benegas
 * @date 2025
 * @details Este archivo contiene pruebas para verificar el correcto funcionamiento
 * de las diferentes estrategias de descuento implementadas en el sistema.
 */

TEST(StrategyTest, PercentageDiscountCalculatesCorrectly)
{
    // 10% discount
    DescuentoPorcentaje strategy(0.10);
    double basePrice = 100.0;
    double expected = 90.0;
    EXPECT_DOUBLE_EQ(strategy.aplicarDescuento(basePrice), expected);
}

TEST(StrategyTest, PercentageDiscountHandlesZero)
{
    DescuentoPorcentaje strategy(0.50);
    double basePrice = 0.0;
    EXPECT_DOUBLE_EQ(strategy.aplicarDescuento(basePrice), 0.0);
}

TEST(StrategyTest, PercentageDiscountHandlesFullDiscount)
{
    DescuentoPorcentaje strategy(1.00); // 100%
    double basePrice = 50.0;
    EXPECT_DOUBLE_EQ(strategy.aplicarDescuento(basePrice), 0.0);
}

TEST(StrategyTest, FixedDiscountCalculatesCorrectly)
{
    // $5 discount
    DescuentoFijo strategy(5.0);
    double basePrice = 20.0;
    double expected = 15.0;
    EXPECT_DOUBLE_EQ(strategy.aplicarDescuento(basePrice), expected);
}

TEST(StrategyTest, FixedDiscountDoesNotGoNegative)
{
    // $20 discount on $10 price
    DescuentoFijo strategy(20.0);
    double basePrice = 10.0;
    // Suponiendo que la lógica impide los valores negativos o devuelve el resultado.
    // Normalmente, el descuento no debería hacer que el precio sea negativo.
    // Verifiquemos el comportamiento. Si la implementación devuelve un valor negativo, lo veremos.
    // Pero normalmente el precio mínimo es 0.
    double result = strategy.aplicarDescuento(basePrice);
    EXPECT_GE(result, 0.0);
}

TEST(StrategyTest, NullDiscountDoesNothing)
{
    DescuentoNulo strategy;
    double basePrice = 123.45;
    EXPECT_DOUBLE_EQ(strategy.aplicarDescuento(basePrice), basePrice);
}
