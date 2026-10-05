#include <gtest/gtest.h>

#include "../common/include/color.hpp"
#include "../common/include/parse_config.hpp"
#include "../common/include/point.hpp"
#include "../common/include/pov.hpp"
#include "../common/include/scene.hpp"
#include "../common/include/vector.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

namespace {

  // Helper: ejecuta el parser sobre un string
  void parse_into_scene(std::string const & cfg, render::Scene & scene) {
    std::istringstream iss(cfg);
    parse::parse_config_stream(iss, scene);
  }

}  // namespace

// =============== Tests de éxito ===============

TEST(parse_config, parses_basic_pov_render_background_and_seeds) {
  // Incluye: aspect_ratio + image_width -> fija tamaño imagen
  // cámara, FOV, render params, colores y seeds
  std::string const cfg = "aspect_ratio: 16 9\n"
                          "image_width: 180\n"
                          "camera_position: 0 1 2\n"
                          "camera_target: 0 0 0\n"
                          "camera_north: 0 1 0\n"
                          "field_of_view: 60\n"
                          "samples_per_pixel: 4\n"
                          "max_depth: 3\n"
                          "gamma: 2.2\n"
                          "background_dark_color: 0.1 0.2 0.3\n"
                          "background_light_color: 0.9 0.8 0.7\n"
                          "material_rng_seed: 123456789\n"
                          "ray_rng_seed: 987654321\n";

  render::Scene scene;
  ASSERT_NO_THROW(parse_into_scene(cfg, scene));

  // POV configurado
  auto const & pov = scene.get_pov();
  // Tamaño de imagen: compute_image_size(180, 16, 9)
  // No conocemos la fórmula exacta, pero sí que el width es 180
  EXPECT_EQ(pov.get_image_width(), 180);
  // Y la altura debe ser > 0
  EXPECT_GT(pov.get_image_height(), 0);

  // Cámara
  EXPECT_DOUBLE_EQ(pov.get_camera_position().get_x(), 0.0);
  EXPECT_DOUBLE_EQ(pov.get_camera_position().get_y(), 1.0);
  EXPECT_DOUBLE_EQ(pov.get_camera_position().get_z(), 2.0);

  EXPECT_DOUBLE_EQ(pov.get_camera_target().get_x(), 0.0);
  EXPECT_DOUBLE_EQ(pov.get_camera_target().get_y(), 0.0);
  EXPECT_DOUBLE_EQ(pov.get_camera_target().get_z(), 0.0);

  // North/up
  EXPECT_DOUBLE_EQ(pov.get_camera_north().get_x(), 0.0);
  EXPECT_DOUBLE_EQ(pov.get_camera_north().get_y(), 1.0);
  EXPECT_DOUBLE_EQ(pov.get_camera_north().get_z(), 0.0);

  // FOV (si hay getter)
  EXPECT_NEAR(pov.get_field_of_view(), 60.0, 1e-12);

  // Render params
  EXPECT_EQ(scene.get_samples_per_pixel(), 4);
  EXPECT_EQ(scene.get_max_depth(), 3);
  EXPECT_NEAR(scene.get_gamma(), 2.2, 1e-12);

  // Seeds
  EXPECT_EQ(scene.get_material_rng_seed(), 123'456'789ULL);
  EXPECT_EQ(scene.get_rays_rng_seed(), 987'654'321ULL);

  // Background
  auto const & dark  = scene.get_background_dark_color();
  auto const & light = scene.get_background_light_color();
  EXPECT_NEAR(dark.get_r(), 0.1, 1e-12);
  EXPECT_NEAR(dark.get_g(), 0.2, 1e-12);
  EXPECT_NEAR(dark.get_b(), 0.3, 1e-12);
  EXPECT_NEAR(light.get_r(), 0.9, 1e-12);
  EXPECT_NEAR(light.get_g(), 0.8, 1e-12);
  EXPECT_NEAR(light.get_b(), 0.7, 1e-12);
}

TEST(parse_config, ignores_comments_and_blank_lines) {
  std::string const cfg = "\n"
                          "  # comentario\n"
                          "aspect_ratio: 4 3   # trailing comment\n"
                          "image_width: 120\n"
                          "\n"
                          "camera_position: 1 2 3\n"
                          "camera_target: 0 0 0\n"
                          "camera_north: 0 1 0\n"
                          "field_of_view: 45\n";

  render::Scene scene;
  ASSERT_NO_THROW(parse_into_scene(cfg, scene));

  auto const & pov = scene.get_pov();
  EXPECT_EQ(pov.get_image_width(), 120);
  EXPECT_GT(pov.get_image_height(), 0);
  EXPECT_NEAR(pov.get_field_of_view(), 45.0, 1e-12);
}

// =============== Tests de error / validaciones ===============

TEST(parse_config, unknown_key_throws_runtime_error) {
  std::string const cfg = "foo_bar: 123\n";
  render::Scene scene;
  EXPECT_THROW(parse_into_scene(cfg, scene), std::runtime_error);
}

TEST(parse_config, aspect_ratio_requires_two_positive_ints) {
  // Falta un token
  std::string const cfg1 = "aspect_ratio: 16\n";
  render::Scene s1;
  EXPECT_THROW(parse_into_scene(cfg1, s1), std::runtime_error);

  // Cero o negativo no permitido
  std::string const cfg2 = "aspect_ratio: 0 9\n";
  render::Scene s2;
  EXPECT_THROW(parse_into_scene(cfg2, s2), std::runtime_error);

  // Extra data después de los dos tokens
  std::string const cfg3 = "aspect_ratio: 16 9 extra\n";
  render::Scene s3;
  EXPECT_THROW(parse_into_scene(cfg3, s3), std::runtime_error);
}

TEST(parse_config, image_width_requires_one_positive_int_and_no_trailing_garbage) {
  // OK: cámara válida + ratio para que compute_image_size se aplique
  std::string const ok = "aspect_ratio: 16 9\n"
                         "image_width: 200\n"
                         "camera_position: 0 0 1\n"
                         "camera_target: 0 0 0\n"
                         "camera_north: 0 1 0\n"
                         "field_of_view: 60\n";

  render::Scene s_ok;
  EXPECT_NO_THROW(parse_into_scene(ok, s_ok));
  EXPECT_EQ(s_ok.get_pov().get_image_width(), 200);

  // No positivo
  std::string const bad1 = "image_width: -5\n";
  render::Scene s1;
  EXPECT_THROW(parse_into_scene(bad1, s1), std::runtime_error);

  // Basura extra
  std::string const bad2 = "image_width: 200 foo\n";
  render::Scene s2;
  EXPECT_THROW(parse_into_scene(bad2, s2), std::runtime_error);
}

TEST(parse_config, camera_position_target_north_require_three_doubles) {
  // Menos de 3 tokens
  std::string const bad = "camera_position: 0 1\n";
  render::Scene s;
  EXPECT_THROW(parse_into_scene(bad, s), std::runtime_error);

  // Extra data
  std::string const bad2 = "camera_target: 0 0 0 extra\n";
  render::Scene s2;
  EXPECT_THROW(parse_into_scene(bad2, s2), std::runtime_error);

  // Correcto mínimo para que el parser no explote
  std::string const ok = "aspect_ratio: 16 9\n"
                         "image_width: 100\n"
                         "camera_position: 1 2 3\n"
                         "camera_target: 0 0 0\n"
                         "camera_north: 0 1 0\n"
                         "field_of_view: 60\n";
  render::Scene s_ok;
  EXPECT_NO_THROW(parse_into_scene(ok, s_ok));
}

TEST(parse_config, field_of_view_must_be_between_0_and_180) {
  // inválidos
  std::string const bad1 = "aspect_ratio: 16 9\n"
                           "image_width: 100\n"
                           "camera_position: 0 0 1\n"
                           "camera_target: 0 0 0\n"
                           "camera_north: 0 1 0\n"
                           "field_of_view: 0\n";
  render::Scene s1;
  EXPECT_THROW(parse_into_scene(bad1, s1), std::runtime_error);

  std::string const bad2 = "aspect_ratio: 16 9\n"
                           "image_width: 100\n"
                           "camera_position: 0 0 1\n"
                           "camera_target: 0 0 0\n"
                           "camera_north: 0 1 0\n"
                           "field_of_view: 180\n";
  render::Scene s2;
  EXPECT_THROW(parse_into_scene(bad2, s2), std::runtime_error);

  // OK: cámara válida y FOV en rango
  std::string const ok = "aspect_ratio: 16 9\n"
                         "image_width: 100\n"
                         "camera_position: 0 0 1\n"  // pos != target
                         "camera_target: 0 0 0\n"
                         "camera_north: 0 1 0\n"  // no colineal con forward
                         "field_of_view: 59.5\n";

  render::Scene s_ok;
  EXPECT_NO_THROW(parse_into_scene(ok, s_ok));
  EXPECT_NEAR(s_ok.get_pov().get_field_of_view(), 59.5, 1e-12);
}

TEST(parse_config, render_params_must_be_positive_and_gamma_positive) {
  // samples_per_pixel
  {
    std::string const bad = "samples_per_pixel: 0\n";
    render::Scene s;
    EXPECT_THROW(parse_into_scene(bad, s), std::runtime_error);
  }
  // max_depth
  {
    std::string const bad = "max_depth: -1\n";
    render::Scene s;
    EXPECT_THROW(parse_into_scene(bad, s), std::runtime_error);
  }
  // gamma
  {
    std::string const bad = "gamma: 0.0\n";
    render::Scene s;
    EXPECT_THROW(parse_into_scene(bad, s), std::runtime_error);
  }
  // OK
  {
    std::string const ok = "samples_per_pixel: 2\n"
                           "max_depth: 3\n"
                           "gamma: 2.0\n";
    render::Scene s_ok;
    EXPECT_NO_THROW(parse_into_scene(ok, s_ok));
    EXPECT_EQ(s_ok.get_samples_per_pixel(), 2);
    EXPECT_EQ(s_ok.get_max_depth(), 3);
    EXPECT_NEAR(s_ok.get_gamma(), 2.0, 1e-12);
  }
}

TEST(parse_config, background_colors_must_be_rgb_in_0_1) {
  // Valor fuera de [0,1]
  std::string const bad1 = "background_dark_color: 1.2 0.5 0.5\n";
  render::Scene s1;
  EXPECT_THROW(parse_into_scene(bad1, s1), std::runtime_error);

  std::string const bad2 = "background_light_color: 0.1 -0.1 0.2\n";
  render::Scene s2;
  EXPECT_THROW(parse_into_scene(bad2, s2), std::runtime_error);

  // OK
  std::string const ok = "background_dark_color: 0.0 0.0 1.0\n"
                         "background_light_color: 1.0 1.0 1.0\n";
  render::Scene s_ok;
  EXPECT_NO_THROW(parse_into_scene(ok, s_ok));
  EXPECT_NEAR(s_ok.get_background_dark_color().get_b(), 1.0, 1e-12);
  EXPECT_NEAR(s_ok.get_background_light_color().get_r(), 1.0, 1e-12);
}

TEST(parse_config, seeds_are_uint64) {
  // OK
  std::string const ok = "material_rng_seed: 1234567890123456789\n"
                         "ray_rng_seed: 42\n";
  render::Scene s_ok;
  EXPECT_NO_THROW(parse_into_scene(ok, s_ok));
  EXPECT_EQ(s_ok.get_material_rng_seed(), 1'234'567'890'123'456'789ULL);
  EXPECT_EQ(s_ok.get_rays_rng_seed(), 42ULL);

  // Basura extra debe fallar
  std::string const bad = "ray_rng_seed: 42 extra\n";
  render::Scene s_bad;
  EXPECT_THROW(parse_into_scene(bad, s_bad), std::runtime_error);
}

TEST(parse_config, full_minimal_valid_config_builds_pov_in_scene_even_if_partial) {
  // Si solo das aspect_ratio + image_width y cámara + fov, debe poner el POV y tamaño
  std::string const cfg = "aspect_ratio: 1 1\n"
                          "image_width: 10\n"
                          "camera_position: 0 0 -5\n"
                          "camera_target: 0 0 0\n"
                          "camera_north: 0 1 0\n"
                          "field_of_view: 50\n";

  render::Scene scene;
  ASSERT_NO_THROW(parse_into_scene(cfg, scene));
  auto const & pov = scene.get_pov();
  EXPECT_EQ(pov.get_image_width(), 10);
  EXPECT_GT(pov.get_image_height(), 0);
  EXPECT_NEAR(pov.get_field_of_view(), 50.0, 1e-12);
}
