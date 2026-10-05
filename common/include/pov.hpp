#ifndef RENDER_POV_HPP
#define RENDER_POV_HPP

#include "../include/point.hpp"
#include "../include/proyection_window.hpp"
#include "../include/vector.hpp"
#include <cmath>
#include <cstdint>

namespace render {

  struct ImageSize {
    int image_width  = 1'920;
    int image_height = 1'080;
  };

  class Pov {
  public:
    // Default values
    Pov()
        : camera_position{0.0, 0.0, -10.0}, camera_target{0.0, 0.0, 0.0},
          camera_north{0.0, 1.0, 0.0}, field_of_view{90.0}, image_size{}, ray_seed{19},
          proyection_window{Proyection_window(pw_focal_vector(), pw_focal_distance(), pw_height(),
                                              pw_width(), pw_horizontal_vector(),
                                              pw_vertical_vector(), pw_origin())} { }

    // (existing ctor remains)
    Pov(Point camera_position, Point camera_target, Vector camera_north, double field_of_view,
        ImageSize image_size)
        : camera_position{camera_position}, camera_target{camera_target},
          camera_north{camera_north}, field_of_view{field_of_view}, image_size{image_size},
          proyection_window{Proyection_window(pw_focal_vector(), pw_focal_distance(), pw_height(),
                                              pw_width(), pw_horizontal_vector(),
                                              pw_vertical_vector(), pw_origin())} { }

    // Getters
    [[nodiscard]] Point get_camera_position() const { return camera_position; }

    [[nodiscard]] Point get_camera_target() const { return camera_target; }

    [[nodiscard]] Vector get_camera_north() const { return camera_north; }

    [[nodiscard]] double get_field_of_view() const { return field_of_view; }

    [[nodiscard]] int get_image_height() const { return image_size.image_height; }

    [[nodiscard]] int get_image_width() const { return image_size.image_width; }

    [[nodiscard]] std::uint64_t get_ray_seed() const { return ray_seed; }

    [[nodiscard]] Proyection_window get_proyection_window() const { return proyection_window; }

    // Setters
    void set_camera_position(Point const & p) {
      camera_position = p;
      recompute_proyection_window();
    }

    void set_camera_target(Point const & t) {
      camera_target = t;
      recompute_proyection_window();
    }

    void set_camera_north(Vector const & n) {
      camera_north = n;
      recompute_proyection_window();
    }

    void set_field_of_view(double f) {
      field_of_view = f;
      recompute_proyection_window();
    }

    void set_image_size(ImageSize const & s) {
      image_size = s;
      recompute_proyection_window();
    }

    void set_ray_seed(std::uint64_t seed) { ray_seed = seed; }

    [[nodiscard]] static ImageSize compute_image_size(int image_width, int aspect_ratio_w,
                                                      int aspect_ratio_h);

    // Proyection window
    [[nodiscard]] Vector pw_focal_vector() const {
      return camera_position.substract(camera_target);
    }

    [[nodiscard]] double pw_focal_distance() const;
    [[nodiscard]] double pw_height() const;
    [[nodiscard]] double pw_width() const;
    [[nodiscard]] Vector pw_director_vector_u() const;
    [[nodiscard]] Vector pw_director_vector_v() const;
    [[nodiscard]] Vector pw_horizontal_vector() const;
    [[nodiscard]] Vector pw_vertical_vector() const;
    [[nodiscard]] Point pw_origin() const;

  private:
    Point camera_position;
    Point camera_target;
    Vector camera_north;
    double field_of_view;
    ImageSize image_size;
    std::uint64_t ray_seed{};
    Proyection_window proyection_window;

    // Helper para recomponer la Proyection_window tras cambios o en el ctor por defecto
    void recompute_proyection_window() {
      proyection_window =
          Proyection_window(pw_focal_vector(), pw_focal_distance(), pw_height(), pw_width(),
                            pw_horizontal_vector(), pw_vertical_vector(), pw_origin());
    }
  };

}  // namespace render

#endif
