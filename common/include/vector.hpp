#ifndef RENDER_VECTOR_HPP
#define RENDER_VECTOR_HPP

#include <cmath>

namespace render {

  class Vector {
  public:
    Vector(double cx, double cy, double cz) : x{cx}, y{cy}, z{cz} { }

    // Getters
    [[nodiscard]] double get_x() const { return x; }

    [[nodiscard]] double get_y() const { return y; }

    [[nodiscard]] double get_z() const { return z; }

    // Operators
    [[nodiscard]] double magnitude() const;
    [[nodiscard]] Vector normalized() const;
    [[nodiscard]] Vector perpendicular_component(Vector const & other) const;

    [[nodiscard]] Vector add(Vector const & other) const {
      return {x + other.x, y + other.y, z + other.z};
    }

    [[nodiscard]] Vector substract(Vector const & other) const {
      return {x - other.x, y - other.y, z - other.z};
    }

    [[nodiscard]] Vector dot(double scalar) const { return {x * scalar, y * scalar, z * scalar}; }

    // Version que modifica el propio vector y así no crea una copia innecesaria
    Vector & dot_in_place(double scalar) {
      x = x * scalar;
      y = y * scalar;
      z = z * scalar;
      return *this;
    }

    [[nodiscard]] double dot(Vector const & other) const {
      return x * other.x + y * other.y + z * other.z;
    }

    [[nodiscard]] Vector cross(Vector const & other) const {
      return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }

  private:
    double x, y, z;
  };

}  // namespace render

#endif
