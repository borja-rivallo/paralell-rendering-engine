#include "../common/include/color.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <stdexcept>

namespace {

  // Pruebas del constructor de Color con sus getters

  // Caso de prueba función miembro: constructor con inicialización correcta
  TEST(test_color, constructor_valid_inicialization) {
    render::Color const color(0.5, 0.5, 0.5);
    EXPECT_DOUBLE_EQ(color.get_r(), 0.5);
    EXPECT_DOUBLE_EQ(color.get_g(), 0.5);
    EXPECT_DOUBLE_EQ(color.get_b(), 0.5);
  }

  // Caso de prueba función miembro: constructor con inicialización inválida r, g, b menores que 0.0
  TEST(test_color, constructor_invalid_inicialization) {
    // Caso r < 0.0
    EXPECT_THROW(render::Color(-0.1, 0.5, 0.5), std::runtime_error);
    // Caso g < 0.0
    EXPECT_THROW(render::Color(0.5, -2.0, 0.5), std::runtime_error);
    // Caso b < 0.0
    EXPECT_THROW(render::Color(0.5, 0.5, -0.5), std::runtime_error);
  }

  // Caso de prueba función miembro: constructor con inicialización inválida r, g, b mayores que 1.0
  TEST(test_color, constructor_invalid_inicialization_above_one) {
    // Caso r > 1.0
    EXPECT_THROW(render::Color(1.1, 0.5, 0.5), std::runtime_error);
    // Caso g > 1.0
    EXPECT_THROW(render::Color(0.5, 2.0, 0.5), std::runtime_error);
    // Caso b > 1.0
    EXPECT_THROW(render::Color(0.5, 0.5, 1.5), std::runtime_error);
  }

  // Pruebas de los métodos de la clase Color

  // Caso de prueba función miembro: multiply
  TEST(test_color, multiply) {
    render::Color const color(0.4, 0.8, 0.6);
    render::Color const result = color.multiply(1.0);
    EXPECT_NEAR(result.get_r(), 0.4, 1e-9);
    EXPECT_NEAR(result.get_g(), 0.8, 1e-9);
    EXPECT_NEAR(
        result.get_b(), 0.6,
        1e-9);  // En este caso no va a lanzar error porque la validación es solo en el constructor
    EXPECT_DOUBLE_EQ(color.get_r(), 0.4);  // Verifica que el color original no cambió
  }

  // Caso de error función miembro: multiply con factor negativo
  TEST(test_color, multiply_negative_factor) {
    render::Color const color(0.2, 0.4, 0.6);
    EXPECT_THROW(
        {
          render::Color const result = color.multiply(-1.0);
          (void) result;
        },
        std::runtime_error);
  }

  // Caso de error función miembro: multiply con factor muy grande
  TEST(test_color, multiply_large_factor) {
    render::Color const color(0.2, 0.4, 0.6);
    EXPECT_THROW(
        {
          render::Color const result = color.multiply(10.0);
          (void) result;
        },
        std::runtime_error);
  }

  // Caso de prueba función miembro: add
  TEST(test_color, add) {
    render::Color const color1(0.2, 0.3, 0.4);
    render::Color const color2(0.5, 0.4, 0.3);
    render::Color const result = color1.add(color2);
    EXPECT_DOUBLE_EQ(result.get_r(), 0.7);
    EXPECT_DOUBLE_EQ(result.get_g(), 0.7);
    EXPECT_DOUBLE_EQ(result.get_b(), 0.7);
    EXPECT_DOUBLE_EQ(color1.get_r(), 0.2);  // Verifica que el color original no cambió
  }

  // Caso de error función miembro: add que excede el rango máximo
  TEST(test_color, add_exceeding_range) {
    render::Color const color1(0.8, 0.9, 1.0);
    render::Color const color2(0.5, 0.4, 0.3);
    EXPECT_THROW(
        {
          render::Color const result = color1.add(color2);
          (void) result;
        },
        std::runtime_error);
  }

  // Caso de prueba función miembro: multiply_in_place usando factor double. Para este caso no se
  // lanza error aunque los valores excedan 1.0
  TEST(test_color, multiply_in_place) {
    render::Color color(0.2, 0.4, 0.6);
    color.multiply_in_place(2.0);
    EXPECT_DOUBLE_EQ(color.get_r(), 0.4);
    EXPECT_DOUBLE_EQ(color.get_g(), 0.8);
    EXPECT_DOUBLE_EQ(
        color.get_b(),
        1.2);  // En este caso no va a lanzar error porque la validación es solo en el constructor
  }

  // Caso de prueba función miembro: multiply_in_place usando otro Color. Para este caso no se lanza
  // error aunque los valores excedan 1.0
  TEST(test_color, multiply_in_place_color) {
    render::Color color1(0.2, 0.4, 0.6);
    render::Color const color2(0.5, 0.5, 1.0);
    EXPECT_NO_THROW(color1.multiply_in_place(color2));
    EXPECT_NEAR(color1.get_r(), 0.1, 1e-9);
    EXPECT_NEAR(color1.get_g(), 0.2, 1e-9);
    EXPECT_NEAR(color1.get_b(), 0.6, 1e-9);
  }

  // Caso de prueba función miembro: add_in_place. Para este caso no se lanza error aunque los
  // valores excedan 1.0
  TEST(test_color, add_in_place) {
    render::Color color1(0.2, 0.3, 0.4);
    render::Color const color2(0.5, 0.4, 0.3);
    color1.add_in_place(color2);
    EXPECT_DOUBLE_EQ(color1.get_r(), 0.7);
    EXPECT_DOUBLE_EQ(color1.get_g(), 0.7);
    EXPECT_DOUBLE_EQ(color1.get_b(), 0.7);
  }

  // Caso de prueba función miembro: apply_gamma_correction
  TEST(test_color, apply_gamma_correction) {
    render::Color color(0.25, 0.5, 0.75);
    color.apply_gamma_correction(2.0);
    EXPECT_DOUBLE_EQ(color.get_r(), 0.5);
    EXPECT_DOUBLE_EQ(color.get_g(), std::sqrt(0.5));
    EXPECT_DOUBLE_EQ(color.get_b(), std::sqrt(0.75));
  }

  // Caso de error función miembro: apply_gamma_correction con gamma negativo
  TEST(test_color, apply_gamma_correction_negative) {
    render::Color color(0.25, 0.5, 0.75);
    EXPECT_THROW(color.apply_gamma_correction(-2.0), std::runtime_error);
  }

  // Caso de error función miembro: apply_gamma_correction con gamma cero
  TEST(test_color, apply_gamma_correction_zero) {
    render::Color color(0.25, 0.5, 0.75);
    EXPECT_THROW(color.apply_gamma_correction(0.0), std::runtime_error);
  }

}  // namespace
