#ifndef RENDER_POINT_HPP
#define RENDER_POINT_HPP

#include "../include/vector.hpp"
#include <cmath>

namespace render {

  class Point {
  public:
    Point(double cx, double cy, double cz) : x{cx}, y{cy}, z{cz} { }

    // Getters
    [[nodiscard]] double get_x() const { return x; }

    [[nodiscard]] double get_y() const { return y; }

    [[nodiscard]] double get_z() const { return z; }

    // Operators
    [[nodiscard]] Vector substract(Point const & other) const {
      return Vector{x - other.x, y - other.y, z - other.z};
    }

    [[nodiscard]] Point substract(Vector const & other) const {
      return Point{x - other.get_x(), y - other.get_y(), z - other.get_z()};
    }

    [[nodiscard]] Vector add(Point const & other) const {
      return Vector{x + other.x, y + other.y, z + other.z};
    }

    [[nodiscard]] Point add(Vector const & other) const {
      return Point{x + other.get_x(), y + other.get_y(), z + other.get_z()};
    }

  private:
    double x, y, z;
  };

}  // namespace render

#endif
