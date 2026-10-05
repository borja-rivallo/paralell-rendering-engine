#include "../common/include/color.hpp"
#include "../common/include/metal.hpp"
#include <gtest/gtest.h>
#include <string>

namespace {

  // Pruebas del constructor de Metal con sus getters

  // Caso de prueba función miembro: constructor con inicialización correcta
  TEST(test_metal, constructor_valid_initialization) {
    render::Color const color(0.5, 0.5, 0.5);
    std::string const name = "metal1";
    render::Metal const metal(name, color, 0.8);
    EXPECT_EQ(metal.get_name(), name);
    EXPECT_DOUBLE_EQ(metal.get_reflectance().get_r(), 0.5);
    EXPECT_DOUBLE_EQ(metal.get_reflectance().get_g(), 0.5);
    EXPECT_DOUBLE_EQ(metal.get_reflectance().get_b(), 0.5);
    EXPECT_DOUBLE_EQ(metal.get_difusion_factor(), 0.8);
  }

  // Caso de prueba función miembro: constructor con inicialización inválida por difusión fuera de
  // rango
  TEST(test_metal, constructor_invalid_initialization_out_of_range) {
    render::Color const color(0.5, 0.5, 0.5);
    std::string const name = "metal_invalid";
    EXPECT_NO_THROW({ render::Metal const metal(name, color, 1.5); });
  }

}  // namespace
