#ifndef RENDER_REFRACTIVE_HPP
#define RENDER_REFRACTIVE_HPP

#include <stdexcept>
#include <string>
#include <utility>

namespace render {

  class Refractive {
  public:
    Refractive(std::string name, double refraction_index)
        : name{std::move(name)}, refraction_index(refraction_index) {
      if (refraction_index < 0.0) {
        throw std::runtime_error("Error: Invalid refractive index");
      }
    }

    // Getters
    [[nodiscard]] std::string get_name() const { return name; }

    [[nodiscard]] double get_refraction_index() const { return refraction_index; }

  private:
    std::string name;
    double refraction_index;
  };

}  // namespace render

#endif
