#include "../common/include/point.hpp"
#include "../common/include/vector.hpp"
#include <gtest/gtest.h>

namespace {

  render::Point const P1{1.0, 2.0, 3.0};
  render::Point const P2{5.0, 1.0, 7.0};
  render::Vector const V1{2.0, 3.0, 4.0};

  // Pruebas del constructor y getters

  // Caso de prueba: correcta inicialización constructor
  TEST(test_point, constructor_and_getters_return_correct_values) {
    render::Point const point{10.5, -2.0, 0.0};

    EXPECT_DOUBLE_EQ(point.get_x(), 10.5);
    EXPECT_DOUBLE_EQ(point.get_y(), -2.0);
    EXPECT_DOUBLE_EQ(point.get_z(), 0.0);
  }

  // Pruebas aritmética entre punto-punto resultado vector

  // Caso de prueba: suma de dos puntos
  TEST(test_point, add_point_returns_correct_vector) {
    render::Vector const result = P1.add(P2);  // {1+5, 2+1, 3+7} = {6, 3, 10}

    EXPECT_DOUBLE_EQ(result.get_x(), 6.0);
    EXPECT_DOUBLE_EQ(result.get_y(), 3.0);
    EXPECT_DOUBLE_EQ(result.get_z(), 10.0);
  }

  // Caso de prueba: resta de dos puntos
  TEST(test_point, substract_point_returns_correct_vector) {
    render::Vector const result = P2.substract(P1);  // {5-1, 1-2, 7-3} = {4, -1, 4}

    EXPECT_DOUBLE_EQ(result.get_x(), 4.0);
    EXPECT_DOUBLE_EQ(result.get_y(), -1.0);
    EXPECT_DOUBLE_EQ(result.get_z(), 4.0);
  }

  // Pruebas aritméticas entre punto y vector resultado punto

  // Caso de prueba: resta entre punto y vector
  TEST(test_point, substract_vector_returns_correct_point) {
    render::Point const result = P2.substract(V1);  // P2 - V1 = {5-2, 1-3, 7-4} = {3, -2, 3}

    EXPECT_DOUBLE_EQ(result.get_x(), 3.0);
    EXPECT_DOUBLE_EQ(result.get_y(), -2.0);
    EXPECT_DOUBLE_EQ(result.get_z(), 3.0);
  }

  // Caso de prueba: suma entre punto y vector
  TEST(test_point, add_vector_returns_correct_point) {
    render::Point const result = P1.add(V1);  // P1 + V1 = {1+2, 2+3, 3+4} = {3, 5, 7}

    EXPECT_DOUBLE_EQ(result.get_x(), 3.0);
    EXPECT_DOUBLE_EQ(result.get_y(), 5.0);
    EXPECT_DOUBLE_EQ(result.get_z(), 7.0);
  }

}  // namespace
