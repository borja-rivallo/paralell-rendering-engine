#include "../common/include/point.hpp"
#include "../common/include/proyection_window.hpp"
#include "../common/include/vector.hpp"
#include <gtest/gtest.h>

namespace {

  render::Vector const V_FOCAL{0.0, 0.0, -10.0};
  double const DISTANCE_FOCAL = 10.0;
  double const HEIGHT         = 20.0;
  double const WIDTH          = 35.5;
  render::Vector const V_HORIZONTAL{-35.5, 0.0, 0.0};
  render::Vector const V_VERTICAL{0.0, -20.0, 0.0};
  render::Point const P_ORIGIN{17.75, 10.0, 0.0};

  // Pruebas para constructor y getters

  // Caso de prueba: correcta inicialización del constructor
  TEST(test_proyection_window, constructor_initializes_all_members) {
    // Inicialización
    render::Proyection_window const pw(V_FOCAL, DISTANCE_FOCAL, HEIGHT, WIDTH, V_HORIZONTAL, V_VERTICAL,
                                 P_ORIGIN);

    // Vectores y Puntos
    render::Vector const test_vf    = pw.get_focal_vector();
    render::Vector const test_vh    = pw.get_horizontal_vector();
    render::Point const test_origin = pw.get_origin();

    // Verificación de focal_vector
    EXPECT_DOUBLE_EQ(test_vf.get_z(), -10.0);

    // Verificación de horizontal_vector
    EXPECT_DOUBLE_EQ(test_vh.get_x(), -35.5);

    // Verificación de origin
    EXPECT_DOUBLE_EQ(test_origin.get_x(), 17.75);

    // Verificación de doubles
    EXPECT_DOUBLE_EQ(pw.get_focal_distance(), DISTANCE_FOCAL);
    EXPECT_DOUBLE_EQ(pw.get_height(), HEIGHT);
    EXPECT_DOUBLE_EQ(pw.get_width(), WIDTH);
  }

  // Caso de prueba: getters de vectores y puntos devuleven valor exacto
  TEST(test_proyection_window, getters_return_input_values) {
    // Inicialización con valores distintos a los de la prueba anterior
    render::Proyection_window const pw(V_VERTICAL, 5.0, 10.0, 50.0, V_FOCAL, V_FOCAL, P_ORIGIN);

    // Verificar que los vectores se asignaron a los campos correctos
    render::Vector const test_focal = pw.get_focal_vector();
    EXPECT_DOUBLE_EQ(test_focal.get_y(),
                     -20.0);  // V_FOCAL es {0, 0, -10}. V_VERTICAL es {0, -20, 0}
                              // La inicialización es V_VERTICAL para focal_vector
                              // El valor de V_VERTICAL es {0, -20, 0} si la definicion es correcta

    // Verificamos el valor de distancia focal (5.0)
    EXPECT_DOUBLE_EQ(pw.get_focal_distance(), 5.0);

    // Verificamos el valor de ancho (50.0)
    EXPECT_DOUBLE_EQ(pw.get_width(), 50.0);

    // Verificamos el origen
    EXPECT_DOUBLE_EQ(pw.get_origin().get_y(), 10.0);  // P_ORIGIN es {17.75, 10.0, 0.0}
  }

}  // namespace
