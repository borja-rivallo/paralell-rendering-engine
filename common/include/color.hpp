#ifndef RENDER_COLOR_HPP
#define RENDER_COLOR_HPP
#include <cmath>
#include <stdexcept>

namespace render {

  class Color {
  public:
    Color(double r, double g, double b) : r(r), g(g), b(b) {
      if (r < 0.0 or r > 1.0 or g < 0.0 or g > 1.0 or b < 0.0 or b > 1.0) {
        throw std::runtime_error("Error: Invalid color parameters");
      }
    }

    // Getters
    [[nodiscard]] double get_r() const { return r; }

    [[nodiscard]] double get_g() const { return g; }

    [[nodiscard]] double get_b() const { return b; }

    // Operators
    [[nodiscard]] Color multiply(double factor) const {
      return {r * factor, g * factor, b * factor};
    }

    // Esta versión del multiply es para cuando no quieras crear un objeto color temporal
    Color & multiply_in_place(double factor) {
      r *= factor;
      g *= factor;
      b *= factor;
      return *this;
    }

    // Sobrecarga del método que no crea color temporal
    Color & multiply_in_place(Color const & other) {
      r *= other.r;
      g *= other.g;
      b *= other.b;
      return *this;
    }

    [[nodiscard]] Color add(Color const & other) const {
      return {r + other.r, g + other.g, b + other.b};
    }

    // Versión de la suma que no crea color temporal
    Color & add_in_place(Color const & other) {
      r += other.r;
      g += other.g;
      b += other.b;
      return *this;
    }

    // En este tampoco se crea color temporal
    Color & apply_gamma_correction(double gamma) {
      if (gamma <= 0.0) {
        throw std::runtime_error("Error: Invalid gamma value");
      }
      double const inv_gamma = 1.0 / gamma;
      r                      = std::pow(r, inv_gamma);
      g                      = std::pow(g, inv_gamma);
      b                      = std::pow(b, inv_gamma);
      return *this;
    }

  private:
    double r;
    double g;
    double b;
  };

}  // namespace render

#endif
