#ifndef RENDER_SPHERE_HPP
#define RENDER_SPHERE_HPP

#include "../include/matte.hpp"
#include "../include/metal.hpp"
#include "../include/point.hpp"
#include "../include/refractive.hpp"
#include <utility>
#include <variant>

namespace render {

  using t_material = std::variant<Matte, Metal, Refractive>;

  class Sphere {
  public:
    Sphere(Point sphere_center, double radius, t_material material)
        : sphere_center{sphere_center}, radius{radius}, material(std::move(material)) {
      if (radius <= 0.0) {
        throw std::runtime_error("Error: Invalid sphere parameters");
      }
    }

    [[nodiscard]] double get_radius() const { return radius; }

    [[nodiscard]] Point get_center() const { return sphere_center; }

    [[nodiscard]] t_material get_material() const { return material; }

  private:
    Point sphere_center;
    double radius;
    t_material material;
  };

}  // namespace render

#endif
