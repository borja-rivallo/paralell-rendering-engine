#ifndef RENDER_CYLINDER_HPP
#define RENDER_CYLINDER_HPP

#include "../include/matte.hpp"
#include "../include/metal.hpp"
#include "../include/point.hpp"
#include "../include/refractive.hpp"
#include "../include/vector.hpp"
#include <utility>
#include <variant>

namespace render {

  using t_material = std::variant<Matte, Metal, Refractive>;

  class Cylinder {
  public:
    Cylinder(Point vec_center, double radius, Vector vec, t_material material)
        : vec_center{vec_center}, radius{radius}, vec_edge{vec}, material(std::move(material)) {
      if (radius < 0.0) {
        throw std::runtime_error("Error: Invalid cylinder radius");
      }
      if (vec_edge.magnitude() == 0.0) {
        throw std::runtime_error("Error: Invalid cylinder edge vector");
      }
    }

    // Getters
    [[nodiscard]] Point get_center() const { return vec_center; }

    [[nodiscard]] Vector get_edge() const { return vec_edge; }

    [[nodiscard]] double get_radius() const { return radius; }

    [[nodiscard]] double get_height() const { return vec_edge.magnitude(); }

    [[nodiscard]] t_material get_material() const { return material; }

  private:
    Point vec_center;
    double radius;
    Vector vec_edge;
    t_material material;
  };

}  // namespace render

#endif
