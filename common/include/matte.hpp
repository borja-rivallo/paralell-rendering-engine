#ifndef RENDER_MATTE_HPP
#define RENDER_MATTE_HPP

#include "../include/color.hpp"
#include <string>
#include <utility>

namespace render {

  class Matte {
  public:
    Matte(std::string name, Color reflectance) : name{std::move(name)}, reflectance{reflectance} { }

    // Getters
    [[nodiscard]] std::string get_name() const { return name; }

    [[nodiscard]] Color get_reflectance() const { return reflectance; }

  private:
    std::string name;
    Color reflectance;
  };

}  // namespace render

#endif
