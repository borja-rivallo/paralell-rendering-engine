#include "../common/include/refractive.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>

namespace {

  // Índices de refracción de prueba
  double const VALID_IOR = 1.33;
  double const IOR_AIR   = 1.0;

  // Pruebas para constructor y getters de Refractive

  // Caso de prueba: correcta inicialización del constructor con valores válidos
  TEST(test_refractive, constructor_valid_initialization) {
    // IOR > 1.0 (Límite superior de validación en el parser)
    std::string const test_name = "water";

    EXPECT_NO_THROW({ render::Refractive const r(test_name, VALID_IOR); });

    render::Refractive const r(test_name, VALID_IOR);
    EXPECT_EQ(r.get_name(), "water");
    EXPECT_NEAR(r.get_refraction_index(), VALID_IOR, 1e-9);
  }

  // Caso de prueba: IOR = 1.0 (Límite inferior válido)
  TEST(test_refractive, constructor_ior_is_one_is_valid) {
    EXPECT_NO_THROW({ render::Refractive const r("air", IOR_AIR); });
  }

  // Caso de prueba: IOR < 1.0 (Debe lanzar error)
  TEST(test_refractive, constructor_negative_ior_throws) {
    double const negative_ior = -0.001;
    EXPECT_THROW({ render::Refractive const r("negative", negative_ior); }, std::runtime_error);
  }

}  // namespace
