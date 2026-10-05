#include "../include/scene.hpp"
#include <istream>

namespace render {

  void dispatch_scene_entity(std::string const & tag, std::vector<std::string> const & tokens,
                             Scene & scene, std::string const & line_content);

}  // namespace render

namespace parse {

  void parse_scene_stream(std::istream & in, render::Scene & scene);

}  // namespace parse
