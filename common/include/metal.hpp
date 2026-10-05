#ifndef RENDER_METAL_HPP
#define RENDER_METAL_HPP

#include "../include/color.hpp"
#include <string>
#include <utility>

namespace render {

  class Metal {
  public:
    Metal(std::string name, Color reflectance, double difusion_factor)
        : name{std::move(name)}, reflectance(reflectance), difusion_factor(difusion_factor) { }

    // Getters
    [[nodiscard]] std::string get_name() const { return name; }

    [[nodiscard]] Color get_reflectance() const { return reflectance; }

    [[nodiscard]] double get_difusion_factor() const { return difusion_factor; }

  private:
    std::string name;
    Color reflectance;
    double difusion_factor;
  };

}  // namespace render

#endif
