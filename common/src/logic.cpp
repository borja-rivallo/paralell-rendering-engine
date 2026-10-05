#include "../include/logic.hpp"
#include "../include/parse_config.hpp"
#include "../include/parse_scene.hpp"
#include "../include/scene.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace render {

  void validate_arguments(int argc, std::vector<std::string> const & arguments) {
    if (argc != 4) {
      throw std::invalid_argument("Error: Invalid number of arguments: " +
                                  std::to_string(argc - 1) +
                                  "\nUsage: " +
                                  arguments[0] +
                                  " <config_file> <scene_file> <output_file>");
    }
  }

  bool load_scene_from_files(render::Scene & scene, std::ifstream & in, std::ifstream & file) {
    parse::parse_scene_stream(in, scene);

    parse::parse_config_stream(file, scene);
    return true;
  }

  Scene load_scene(std::string const & config_path, std::string const & scene_path) {
    std::ifstream config_file(config_path);
    if (!config_file) {
      throw std::runtime_error("No se pudo abrir: " + config_path);
    }

    std::ifstream scene_file(scene_path);
    if (!scene_file) {
      throw std::runtime_error("No se pudo abrir: " + scene_path);
    }

    Scene scene;
    load_scene_from_files(scene, scene_file, config_file);
    return scene;
  }

  void write_ppm_header(std::ofstream & file, int width, int height) {
    file << "P3\n" << width << " " << height << "\n255\n";
  }

}  // namespace render
