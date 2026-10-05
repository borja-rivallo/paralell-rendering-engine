#ifndef RENDER_PROYECTION_WINDOW_HPP
#define RENDER_PROYECTION_WINDOW_HPP

#include "../include/point.hpp"
#include "../include/vector.hpp"

namespace render {

  class Proyection_window {
  public:
    Proyection_window(Vector focal_vector, double focal_distance, double height, double width,
                      Vector horizontal_vector, Vector vertical_vector, Point origin)
        : focal_vector{focal_vector}, focal_distance{focal_distance}, height{height}, width{width},
          horizontal_vector{horizontal_vector}, vertical_vector{vertical_vector}, origin{origin} { }

    [[nodiscard]] Vector get_focal_vector() const { return focal_vector; }

    [[nodiscard]] double get_focal_distance() const { return focal_distance; }

    [[nodiscard]] double get_height() const { return height; }

    [[nodiscard]] double get_width() const { return width; }

    [[nodiscard]] Vector get_horizontal_vector() const { return horizontal_vector; }

    [[nodiscard]] Vector get_vertical_vector() const { return vertical_vector; }

    [[nodiscard]] Point get_origin() const { return origin; }

  private:
    Vector focal_vector;
    double focal_distance;
    double height;
    double width;
    Vector horizontal_vector;
    Vector vertical_vector;
    Point origin;
  };

}  // namespace render

#endif
