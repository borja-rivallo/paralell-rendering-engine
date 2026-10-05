#ifndef RENDER_LOGIC_HPP
#define RENDER_LOGIC_HPP

#include "../../common/include/scene.hpp"
#include <fstream>
#include <string>
#include <vector>

namespace render {

  bool load_scene_from_files(render::Scene & scene, std::ifstream & in, std::ifstream & file);

  void validate_arguments(int argc, std::vector<std::string> const & arguments);

  Scene load_scene(std::string const & config_path, std::string const & scene_path);

  void write_ppm_header(std::ofstream & file, int width, int height);

}  // namespace render

#endif
