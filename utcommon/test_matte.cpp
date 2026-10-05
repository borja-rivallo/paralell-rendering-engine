#include "../common/include/color.hpp"
#include "../common/include/matte.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

namespace {

  // Pruebas del constructor de Matte con sus getters

  // Caso de prueba función miembro: constructor con inicialización correcta
  TEST(test_matte, constructor_valid_initialization) {
    render::Color const color(0.5, 0.5, 0.5);
    std::string const name = "matte1";
    render::Matte const matte(name, color);
    EXPECT_EQ(matte.get_name(), name);
    EXPECT_DOUBLE_EQ(matte.get_reflectance().get_r(), 0.5);
    EXPECT_DOUBLE_EQ(matte.get_reflectance().get_g(), 0.5);
    EXPECT_DOUBLE_EQ(matte.get_reflectance().get_b(), 0.5);
  }

  // Caso de prueba función miembro: constructor con inicialización inválida (color fuera de rango)
  TEST(test_matte, constructor_invalid_initialization_out_of_range) {
    std::string const name = "matte_invalid";
    EXPECT_THROW(
        {
          render::Color const invalid_color(1.5, 0.5, 0.5);

          render::Matte const matte(name, invalid_color);
          (void) matte;
        },
        std::runtime_error);
  }

}  // namespace
