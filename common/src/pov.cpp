#include "../include/pov.hpp"
#include <cmath>
#include <numbers>

namespace render {

  constexpr double PI = std::numbers::pi;

  ImageSize Pov::compute_image_size(int image_width, int aspect_ratio_w, int aspect_ratio_h) {
    double const ratio = static_cast<double>(aspect_ratio_h) / static_cast<double>(aspect_ratio_w);
    int const image_height = static_cast<int>(std::floor(image_width * ratio));
    return ImageSize{image_width, image_height};
  }

  [[nodiscard]] double Pov::pw_focal_distance() const {
    return camera_position.substract(camera_target).magnitude();
  }

  [[nodiscard]] double Pov::pw_height() const {
    return 2.0 * pw_focal_distance() * std::tan((field_of_view * PI / 180.0) / 2.0);
  }

  [[nodiscard]] double Pov::pw_width() const {
    return pw_height() * (static_cast<double>(image_size.image_width) /
                          (static_cast<double>(image_size.image_height)));
  }

  [[nodiscard]] Vector Pov::pw_director_vector_u() const {
    return (camera_north.cross(pw_focal_vector().normalized())).normalized();
  }

  [[nodiscard]] Vector Pov::pw_director_vector_v() const {
    return pw_focal_vector().normalized().cross(pw_director_vector_u());
  }

  [[nodiscard]] Vector Pov::pw_horizontal_vector() const {
    return pw_director_vector_u().dot(pw_width());
  }

  [[nodiscard]] Vector Pov::pw_vertical_vector() const {
    return pw_director_vector_v().dot(-1.0).dot(pw_height());
  }

  [[nodiscard]] Point Pov::pw_origin() const {
    Vector const focal_vec = pw_focal_vector();
    Vector const p_h       = pw_horizontal_vector();
    Vector const p_v       = pw_vertical_vector();

    Vector const delta_x = p_h.dot(1.0 / image_size.image_width);
    Vector const delta_y = p_v.dot(1.0 / image_size.image_height);

    return camera_position.substract(focal_vec)
        .substract(p_h.add(p_v).dot(0.5))
        .add(delta_x.add(delta_y).dot(0.5));
  }

}  // namespace render
