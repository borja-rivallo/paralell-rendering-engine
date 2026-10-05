#include "../common/include/parse_scene.hpp"
#include "../common/include/scene.hpp"
#include <gtest/gtest.h>

#include <sstream>
#include <string>

namespace {

  // Función auxiliar para las pruebas
  void parse_into_scene(std::string const & msg, render::Scene & scene) {
    std::istringstream iss(msg);
    parse::parse_scene_stream(iss, scene);
  }

}  // namespace

// Tests con caso de éxito
// Caso de prueba con parámetros correctos en la escena
TEST(parse_scene, scene_correct_values) {
  std::string const scn = "matte: mat1 0 0.8 0.8\n"
                          "metal: metal1 0 0.8 0 2.0\n"
                          "refractive: ref99 1.3\n"
                          "sphere: 0 0 0 0.85 mat1\n"
                          "cylinder: 0 0 0 0.5 5 2.5 -1.25 metal1\n";
  render::Scene scene;
  ASSERT_NO_THROW(parse_into_scene(scn, scene));

  // Comprobamos materiales
  auto const & metals = scene.get_metals();
  EXPECT_EQ(metals.size(), 1);
  EXPECT_EQ(metals.at(0).get_name(), "metal1");
  EXPECT_EQ(metals.at(0).get_difusion_factor(), 2.0);
  EXPECT_EQ(metals.at(0).get_reflectance().get_r(), 0.0);
  EXPECT_EQ(metals.at(0).get_reflectance().get_g(), 0.8);
  EXPECT_EQ(metals.at(0).get_reflectance().get_b(), 0.0);
  auto const & mattes = scene.get_mattes();
  EXPECT_EQ(mattes.size(), 1);
  EXPECT_EQ(mattes.at(0).get_name(), "mat1");
  EXPECT_EQ(mattes.at(0).get_reflectance().get_r(), 0.0);
  EXPECT_EQ(mattes.at(0).get_reflectance().get_g(), 0.8);
  EXPECT_EQ(mattes.at(0).get_reflectance().get_b(), 0.8);
  auto const & refractives = scene.get_refractives();
  EXPECT_EQ(refractives.size(), 1);
  EXPECT_EQ(refractives.at(0).get_name(), "ref99");
  EXPECT_EQ(refractives.at(0).get_refraction_index(), 1.3);
  // Comprobamos objetos
  auto const & spheres = scene.get_spheres();
  EXPECT_EQ(spheres.size(), 1);
  EXPECT_DOUBLE_EQ(spheres.at(0).get_radius(), 0.85);
  EXPECT_DOUBLE_EQ(spheres.at(0).get_center().get_x(), 0.0);
  EXPECT_DOUBLE_EQ(spheres.at(0).get_center().get_y(), 0.0);
  EXPECT_DOUBLE_EQ(spheres.at(0).get_center().get_z(), 0.0);
  EXPECT_EQ(std::get<render::Matte>(spheres.at(0).get_material()).get_name(), "mat1");
  auto const & cylinders = scene.get_cylinders();
  EXPECT_EQ(cylinders.size(), 1);
  EXPECT_DOUBLE_EQ(cylinders.at(0).get_center().get_x(), 0.0);
  EXPECT_DOUBLE_EQ(cylinders.at(0).get_center().get_y(), 0.0);
  EXPECT_DOUBLE_EQ(cylinders.at(0).get_center().get_z(), 0.0);
  EXPECT_DOUBLE_EQ(cylinders.at(0).get_radius(), 0.5);
  EXPECT_DOUBLE_EQ(cylinders.at(0).get_edge().get_x(), 5.0);
  EXPECT_DOUBLE_EQ(cylinders.at(0).get_edge().get_y(), 2.5);
  EXPECT_DOUBLE_EQ(cylinders.at(0).get_edge().get_z(), -1.25);
  EXPECT_EQ(std::get<render::Metal>(cylinders.at(0).get_material()).get_name(), "metal1");
}
