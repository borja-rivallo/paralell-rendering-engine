#include <gtest/gtest.h>

#include "../common/include/logic.hpp"
#include "../common/include/scene.hpp"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>
namespace fs     = std::filesystem;
fs::path const p = fs::current_path();

namespace {

  // Crea un fichero temporal con contenido y devuelve la ruta.
  // Lanza si no puede escribir.
  std::string write_temp_file(std::string const & prefix, std::string const & content) {
    // ruta: /tmp/<prefix>.<pid>.<time>.tmp
    auto pid = static_cast<unsigned long>(::getpid());
    auto now = static_cast<unsigned long>(std::time(nullptr));
    std::string path =
        "/tmp/" + prefix + "." + std::to_string(pid) + "." + std::to_string(now) + ".tmp";
    {
      std::ofstream out(path, std::ios::binary);
      if (!out) {
        throw std::runtime_error("No se pudo crear temp file: " + path);
      }
      out << content;
    }
    return path;
  }

  // Config mínima válida para que parse_config_stream no lance
  // (aspect_ratio + image_width + cámara + fov).
  std::string minimal_valid_config() {
    return "aspect_ratio: 16 9\n"
           "image_width: 120\n"
           "camera_position: 0 0 -5\n"
           "camera_target: 0 0 0\n"
           "camera_north: 0 1 0\n"
           "field_of_view: 60\n";
  }

  // Escena mínima (habitualmente vacía debería ser aceptable).
  // Si tu parser de escenas exigiera mínimo contenido, añade aquí
  // una línea inofensiva válida según tu gramática.
  std::string minimal_scene() {
    return "";  // escena vacía
  }

}  // namespace

// ---------------- validate_arguments ----------------

TEST(logic_validate_arguments, accepts_exactly_three_user_args) {
  std::vector<std::string> const argv = {"prog", "config.txt", "scene.txt", "out.ppm"};
  EXPECT_NO_THROW(render::validate_arguments(static_cast<int>(argv.size()), argv));
}

TEST(logic_validate_arguments, throws_on_wrong_argc_with_usage_message) {
  {
    std::vector<std::string> const argv = {"prog", "only_two"};
    try {
      render::validate_arguments(static_cast<int>(argv.size()), argv);
      FAIL() << "Expected std::invalid_argument";
    } catch (std::invalid_argument const & e) {
      std::string const msg = e.what();
      EXPECT_NE(msg.find("Invalid number of arguments"), std::string::npos);
      EXPECT_NE(msg.find("Usage:"), std::string::npos);
      EXPECT_NE(msg.find("prog"), std::string::npos);  // incluye argv[0]
    } catch (...) {
      FAIL() << "Unexpected exception type";
    }
  }
  {
    std::vector<std::string> const argv = {"prog", "a", "b", "c", "d"};
    EXPECT_THROW(render::validate_arguments(static_cast<int>(argv.size()), argv),
                 std::invalid_argument);
  }
}

// ---------------- load_scene_from_files ----------------

TEST(logic_load_scene_from_files, parses_minimal_scene_and_config) {
  // Crear ficheros temporales
  std::string const scene_path  = write_temp_file("scene_min", minimal_scene());
  std::string const config_path = write_temp_file("config_min", minimal_valid_config());

  render::Scene scene;
  std::ifstream scene_in(scene_path);
  std::ifstream cfg_in(config_path);

  ASSERT_TRUE(scene_in.is_open());
  ASSERT_TRUE(cfg_in.is_open());

  bool const ok = render::load_scene_from_files(scene, scene_in, cfg_in);
  EXPECT_TRUE(ok);

  // Comprobaciones básicas de que el config se aplicó
  auto const & pov = scene.get_pov();
  EXPECT_EQ(pov.get_image_width(), 120);
  EXPECT_GT(pov.get_image_height(), 0);
  EXPECT_NEAR(pov.get_field_of_view(), 60.0, 1e-12);
}

// ---------------- load_scene ----------------

TEST(logic_load_scene, throws_if_config_file_missing) {
  std::string const missing = "/tmp/this_config_does_not_exist.cfg";
  std::string const scene_p = write_temp_file("scene_ok", minimal_scene());
  EXPECT_THROW(render::load_scene(missing, scene_p), std::runtime_error);
}

TEST(logic_load_scene, throws_if_scene_file_missing) {
  std::string const cfg_p   = write_temp_file("config_ok", minimal_valid_config());
  std::string const missing = "/tmp/this_scene_does_not_exist.rtscene";
  EXPECT_THROW(render::load_scene(cfg_p, missing), std::runtime_error);
}

TEST(logic_load_scene, returns_scene_when_both_files_ok) {
  std::string const cfg_p   = write_temp_file("config_ok", minimal_valid_config());
  std::string const scene_p = write_temp_file("scene_ok", minimal_scene());

  render::Scene scene = render::load_scene(cfg_p, scene_p);

  auto const & pov = scene.get_pov();
  EXPECT_EQ(pov.get_image_width(), 120);
  EXPECT_GT(pov.get_image_height(), 0);
  EXPECT_NEAR(pov.get_field_of_view(), 60.0, 1e-12);
}

// ---------------- write_ppm_header ----------------

TEST(logic_write_ppm_header, writes_valid_ppm_header) {
  // output temporal
  std::string const out_p = write_temp_file("ppm_header", "");
  // Abrir en trunc para sobreescribir
  {
    std::ofstream out(out_p, std::ios::trunc bitor std::ios::binary);
    ASSERT_TRUE(out.is_open());
    render::write_ppm_header(out, 320, 180);
  }

  // Leer y verificar
  std::ifstream in(out_p, std::ios::binary);
  ASSERT_TRUE(in.is_open());

  std::string line1, line2, line3;
  std::getline(in, line1);
  std::getline(in, line2);
  std::getline(in, line3);

  EXPECT_EQ(line1, "P3");
  EXPECT_EQ(line2, "320 180");
  EXPECT_EQ(line3, "255");
}
