#include "scene.hpp"
#include <istream>

namespace parse {

  struct KeyVal {
    std::string key;
    std::string val;
    std::string lineforprint;
  };

  struct ImageCfg {
    bool needs_update{false};
    int width{1'920};
    int ar_w{16};
    int ar_h{9};
  };

  void parse_config_stream(std::istream & in, render::Scene & scene);

}  // namespace parse
