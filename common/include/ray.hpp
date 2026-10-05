#ifndef RENDER_RAY_HPP
#define RENDER_RAY_HPP

#include "../include/color.hpp"
#include "../include/cylinder.hpp"
#include "../include/point.hpp"
#include "../include/sphere.hpp"
#include "../include/vector.hpp"
#include <random>

namespace render {

  using t_material = std::variant<Matte, Metal, Refractive>;

  class Ray {
  public:
    Ray(Point const & origin, Vector const & direction, Color intersection_color)
        : origin(origin), direction(direction), point_intersection(0.0, 0.0, 0.0),
          normal_vector(0.0, 0.0, 0.0),
          intersection_material(Matte("default", Color(1.0, 1.0, 1.0))),
          intersection_color(intersection_color), reflected_direction(0.0, 0.0, 0.0) { }

    // Getters
    [[nodiscard]] Point const & get_origin() const { return origin; }

    [[nodiscard]] Vector const & get_direction() const { return direction; }

    [[nodiscard]] Point const & get_point_intersection() const { return point_intersection; }

    [[nodiscard]] Vector const & get_normal_vector() const { return normal_vector; }

    [[nodiscard]] double get_intersection_distance() const { return intersection_distance; }

    [[nodiscard]] t_material const & get_intersection_material() const {
      return intersection_material;
    }

    [[nodiscard]] Color const & get_intersection_color() const { return intersection_color; }

    [[nodiscard]] Vector const & get_reflected_direction() const { return reflected_direction; }

    // Setters
    void set_point_intersection(Point const & point) { point_intersection = point; }

    void set_normal_vector(Vector const & normal) { normal_vector = normal; }

    void set_intersection_distance(double distance) { intersection_distance = distance; }

    void set_intersection_material(t_material const & material) {
      intersection_material = material;
    }

    void set_intersection_color(Color const & color) { intersection_color = color; }

    void set_reflected_direction(Vector const & direction) { reflected_direction = direction; }

    bool sphere_intersection(Sphere const & sphere, bool & front_face_out);
    bool cylinder_side_intersection(Cylinder const & cylinder, bool & front_face_out);
    bool cylinder_upper_base_intersection(Cylinder const & cylinder, bool & front_face_out);
    bool cylinder_lower_base_intersection(Cylinder const & cylinder, bool & front_face_out);

    void color_contribution(Color const & dark_color, Color const & light_color,
                            std::mt19937_64 & rng, bool front_face);
    void matte_color_contribution(std::mt19937_64 & rng);
    void background_color_contribution(Color const & dark_color, Color const & light_color);
    void metal_color_contribution(Metal const & metal, std::mt19937_64 & rng);
    void refractive_color_contribution(Refractive const & refractive, bool front_face);

  private:
    Point origin;
    Vector direction;
    Point point_intersection;
    Vector normal_vector;
    double intersection_distance = -1.0;
    t_material intersection_material;
    Color intersection_color;
    Vector reflected_direction;
  };

}  // namespace render

#endif
