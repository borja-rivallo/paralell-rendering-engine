#include "../include/vector.hpp"
#include <cmath>
#include <stdexcept>

namespace render {

  double Vector::magnitude() const {
    return std::sqrt(x * x + y * y + z * z);
  }

  Vector Vector::normalized() const {
    double const mag = magnitude();
    if (mag == 0) {
      throw std::runtime_error("Cannot normalize zero vector");
    }
    return {x / mag, y / mag, z / mag};
  }

  Vector Vector::perpendicular_component(Vector const & other) const {
    Vector const unit_axis  = other.normalized();
    double const projection = this->dot(unit_axis);
    return this->substract(unit_axis.dot(projection));
  }

}  // namespace render
