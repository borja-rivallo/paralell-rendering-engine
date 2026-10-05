#include "../common/include/color.hpp"
#include "../common/include/cylinder.hpp"
#include "../common/include/matte.hpp"
#include "../common/include/point.hpp"
#include "../common/include/vector.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <variant>

namespace {

  // Pruebas del constructor de Cylinder

  // Función auxiliar para crear matte
  render::Matte create_default_matte() {
    render::Color const white(1.0, 1.0, 1.0);
    return {"default", white};
  }

  // Caso de prueba función miembro: inicialización válida de Cylinder
  TEST(test_cylinder, constructor_valid_initialization) {
    render::Point const center(0.0, 0.0, 0.0);
    render::Vector const axis(0.0, 1.0, 0.0);
    double const radius               = 1.0;
    render::t_material const material = render::t_material{create_default_matte()};
    // If Cylinder does not accept a material in its constructor, omit the material argument.
    EXPECT_NO_THROW({ render::Cylinder const cylinder(center, radius, axis, material); });
  }

  // Caso de error función miembro: inicialización de Cylinder con radio negativo
  TEST(test_cylinder, constructor_invalid_negative_radius) {
    render::Point const center(0.0, 0.0, 0.0);
    render::Vector const axis(0.0, 1.0, 0.0);
    double const radius               = -1.0;
    render::t_material const material = render::t_material{create_default_matte()};
    EXPECT_THROW(
        { render::Cylinder const cylinder(center, radius, axis, material); }, std::runtime_error);
  }

  // Caso de error función miembro: inicialización de Cylinder con vector de eje nulo
  TEST(test_cylinder, constructor_invalid_zero_axis_vector) {
    render::Point const center(0.0, 0.0, 0.0);
    render::Vector const axis(0.0, 0.0, 0.0);
    double const radius               = 1.0;
    render::t_material const material = render::t_material{create_default_matte()};
    EXPECT_THROW(
        { render::Cylinder const cylinder(center, radius, axis, material); }, std::runtime_error);
  }

  // Pruebas de los getters de Cylinder
  // Casos de prueba funciones miembro: getters
  TEST(test_cylinder, getters_return_correct_values) {
    render::Point const center(1.0, 2.0, 3.0);
    render::Vector const axis(0.0, 1.0, 0.0);
    double const radius               = 1.0;
    render::t_material const material = render::t_material{create_default_matte()};
    render::Cylinder const cylinder(center, radius, axis, material);
    EXPECT_DOUBLE_EQ(cylinder.get_center().get_x(), 1.0);
    EXPECT_DOUBLE_EQ(cylinder.get_center().get_y(), 2.0);
    EXPECT_DOUBLE_EQ(cylinder.get_center().get_z(), 3.0);
    EXPECT_DOUBLE_EQ(cylinder.get_radius(), 1.0);
    EXPECT_DOUBLE_EQ(cylinder.get_height(), axis.magnitude());
    EXPECT_DOUBLE_EQ(cylinder.get_edge().get_x(), 0.0);
    EXPECT_DOUBLE_EQ(cylinder.get_edge().get_y(), 1.0);
    EXPECT_DOUBLE_EQ(cylinder.get_edge().get_z(), 0.0);
    EXPECT_TRUE(std::holds_alternative<render::Matte>(cylinder.get_material()));
  }

}  // namespace
