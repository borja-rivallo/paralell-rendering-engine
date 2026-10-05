#include "../common/include/point.hpp"
#include "../common/include/pov.hpp"
#include "../common/include/vector.hpp"
#include <gtest/gtest.h>

namespace {

  render::Point const ORIGIN{0.0, 0.0, -10.0};
  render::Point const TARGET{0.0, 0.0, 0.0};
  render::Vector const NORTH{0.0, 1.0, 0.0};
  double const FOV_90 = 90.0;
  render::ImageSize const IMAGE_SIZE{1'920, 1'080};

  // Pruebas para compute_image_size

  // Caso de prueba: cálculo altura
  TEST(test_pov, compute_image_size_standard_ratio) {
    // Escenario 1: Aspect Ratio 16:9. Width = 1600. Height = 1600 * (9/16) = 900
    render::ImageSize const size1 = render::Pov::compute_image_size(1'600, 16, 9);
    EXPECT_EQ(size1.image_width, 1'600);
    EXPECT_EQ(size1.image_height, 900);

    // Escenario 2: Aspect Ratio 4:3. Width = 800. Height = 800 * (3/4) = 600
    render::ImageSize const size2 = render::Pov::compute_image_size(800, 4, 3);
    EXPECT_EQ(size2.image_width, 800);
    EXPECT_EQ(size2.image_height, 600);

    // Escenario 3: Prueba con truncamiento (std::floor). AR 3:2. Width=100. Height = 100 * (2/3)
    // = 66.66 -> 66
    render::ImageSize const size3 = render::Pov::compute_image_size(100, 3, 2);
    EXPECT_EQ(size3.image_width, 100);
    EXPECT_EQ(size3.image_height, 66);
  }

  // Pruebas cálculo ventan de proyección

  // Creamos instancia POV
  render::Pov create_base_pov(double fov) {
    return {ORIGIN, TARGET, NORTH, fov, IMAGE_SIZE};
  }

  // Caso de prueba: vector focal con FOV 90 grados
  TEST(test_pov, pw_focal_vector_is_correct) {
    render::Pov const pov = create_base_pov(FOV_90);
    render::Vector const focal_vec =
        pov.pw_focal_vector();  // {0, 0, -10} - {0, 0, 0} = {0, 0, -10}

    EXPECT_DOUBLE_EQ(focal_vec.get_x(), 0.0);
    EXPECT_DOUBLE_EQ(focal_vec.get_y(), 0.0);
    EXPECT_DOUBLE_EQ(focal_vec.get_z(), -10.0);
  }

  // Caso de prueba: distancia focal con FOV 90 grados
  TEST(test_pov, pw_focal_distance_is_correct) {
    render::Pov const pov = create_base_pov(FOV_90);
    double const distance = pov.pw_focal_distance();

    EXPECT_DOUBLE_EQ(distance, 10.0);  // ||{0, 0, -10}|| = 10
  }

  // Caso de prueba: altura y anchura de la ventana de proyección con FOV 90 grados
  TEST(test_pov, pw_height_and_width_are_correct_for_fov_90) {
    render::Pov const pov = create_base_pov(FOV_90);
    EXPECT_DOUBLE_EQ(pov.pw_height(), 20.0);               // 2 * 10 * tan(90/2) = 20
    EXPECT_NEAR(pov.pw_width(), 35.55555555555556, 1e-9);  // 20 * (1920/1080) = 35.55...56
  }

  // Caso de prueba: vector desplazamiento horizontal y vertical de la ventana de proyección con FOV
  // 90 grados
  TEST(test_pov, pw_horizontal_and_vertical_vectors_are_correct_for_fov_90) {
    render::Pov const pov               = create_base_pov(FOV_90);
    render::Vector const horizontal_vec = pov.pw_horizontal_vector();
    render::Vector const vertical_vec   = pov.pw_vertical_vector();
    double const pw_w                   = pov.pw_width();
    double const pw_h                   = pov.pw_height();

    // Horizontal (p_h = wp * u). u = {-1, 0, 0}
    // Esperamos: {-wp, 0, 0}
    EXPECT_NEAR(horizontal_vec.get_x(), -pw_w, 1e-9);
    EXPECT_DOUBLE_EQ(horizontal_vec.get_y(), 0.0);

    // Vertical (p_v = -hp * v). v = {0, 1, 0}
    // Esperamos: {0, -hp, 0}
    EXPECT_DOUBLE_EQ(vertical_vec.get_x(), 0.0);
    EXPECT_NEAR(vertical_vec.get_y(), -pw_h, 1e-9);
  }

  // Caso de prueba: set_field_of_view modifica correctamente el FOV
  TEST(test_pov, set_field_of_view_modifies_fov_correctly) {
    render::Pov pov             = create_base_pov(FOV_90);
    double const initial_height = pov.pw_height();

    pov.set_field_of_view(30.0);
    double const new_height = pov.pw_height();
    EXPECT_NE(initial_height, new_height);
    EXPECT_NEAR(new_height, 5.3589838486224544, 1e-9);
  }

  // Pruebas para el origen

  // Caso de prueba: get_origin devuelve el origen correcto
  TEST(test_pov, get_origin_returns_correct_origin) {
    render::Pov const pov      = create_base_pov(FOV_90);
    render::Point const origin = pov.pw_origin();

    // Cálculo del origen esperado
    double const half_width  = pov.pw_width() / 2.0;
    double const half_height = pov.pw_height() / 2.0;
    double const delta_x_half =
        pov.pw_horizontal_vector().magnitude() / pov.get_image_width() / 2.0;
    double const delta_y_half = pov.pw_vertical_vector().magnitude() / pov.get_image_height() / 2.0;

    // Esperado: {half_width - delta_x_half, half_height - delta_y_half, 0.0}
    EXPECT_NEAR(origin.get_x(), half_width - delta_x_half, 1e-9);
    EXPECT_NEAR(origin.get_y(), half_height - delta_y_half, 1e-9);
    EXPECT_DOUBLE_EQ(origin.get_z(), 0.0);
  }

}  // namespace
