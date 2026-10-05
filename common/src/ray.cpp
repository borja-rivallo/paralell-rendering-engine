#include "../include/ray.hpp"
#include "../include/color.hpp"
#include "../include/cylinder.hpp"
#include "../include/matte.hpp"
#include "../include/metal.hpp"
#include "../include/point.hpp"
#include "../include/refractive.hpp"
#include "../include/sphere.hpp"
#include "../include/vector.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>
#include <variant>

namespace render {

  bool Ray::sphere_intersection(Sphere const & sphere, bool & front_face_out) {
    Vector const rc = sphere.get_center().substract(origin);
    double const a  = direction.dot(direction);
    double const b  = -2.0 * direction.dot(rc);
    double const c  = rc.dot(rc) - std::pow(sphere.get_radius(), 2);

    double const discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
      return false;
    }
    double const final_discriminant = std::sqrt(discriminant);
    double const t1                 = (-b - final_discriminant) / (2 * a);
    double const t2                 = (-b + final_discriminant) / (2 * a);

    if (t1 < 1e-3 and t2 < 1e-3) {
      return false;
    }
    if (t1 >= 1e-3 and t2 >= 1e-3) {
      intersection_distance = std::min(t1, t2);
    } else if (t1 >= 1e-3) {
      intersection_distance = t1;
    } else {
      intersection_distance = t2;
    }

    point_intersection = origin.add(direction.dot(intersection_distance));
    normal_vector = point_intersection.substract(sphere.get_center()).dot(1 / sphere.get_radius());

    double const dot_product = normal_vector.dot(direction);
    front_face_out           = (dot_product < 0);

    if (!front_face_out) {
      normal_vector.dot_in_place(-1);
    }
    return true;
  }

  bool Ray::cylinder_side_intersection(Cylinder const & cylinder, bool & front_face_out) {
    Vector const a_hat = cylinder.get_edge().normalized();
    Vector const rc    = origin.substract(cylinder.get_center());
    double const a =
        (direction.perpendicular_component(a_hat)).dot(direction.perpendicular_component(a_hat));
    double const b =
        2.0 * (rc.perpendicular_component(a_hat).dot(direction.perpendicular_component(a_hat)));
    double const c = (rc.perpendicular_component(a_hat)).dot(rc.perpendicular_component(a_hat)) -
                     std::pow(cylinder.get_radius(), 2);
    double const discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
      return false;
    }
    double const final_discriminant = std::sqrt(discriminant);
    double const t1                 = (-b - final_discriminant) / (2 * a);
    double const t2                 = (-b + final_discriminant) / (2 * a);
    if (t1 < 1e-3 and t2 < 1e-3) {
      return false;
    }
    if (t1 >= 1e-3 and t2 >= 1e-3) {
      intersection_distance = std::min(t1, t2);
    } else if (t1 >= 1e-3) {
      intersection_distance = t1;
    } else {
      intersection_distance = t2;
    }

    point_intersection             = origin.add(direction.dot(intersection_distance));
    Vector const point_to_center   = point_intersection.substract(cylinder.get_center());
    double const height_projection = point_to_center.dot(a_hat);
    if (std::abs(height_projection) > (cylinder.get_height() / 2)) {
      return false;
    }
    normal_vector            = point_to_center.perpendicular_component(a_hat);
    double const dot_product = normal_vector.dot(direction);
    front_face_out           = (dot_product < 0);
    if (!front_face_out) {
      normal_vector.dot_in_place(-1);
    }
    return true;
  }

  bool Ray::cylinder_upper_base_intersection(Cylinder const & cylinder, bool & front_face_out) {
    Vector const a_hat = cylinder.get_edge().normalized();
    Point const p      = cylinder.get_center().add(a_hat.dot(cylinder.get_height() / 2));
    normal_vector      = a_hat;
    Vector const rp    = p.substract(origin);

    if (std::abs(direction.dot(normal_vector)) < 1e-8) {
      return false;
    }

    intersection_distance = rp.dot(normal_vector) / direction.dot(normal_vector);
    if (intersection_distance < 1e-3) {
      return false;
    }

    point_intersection = origin.add(direction.dot(intersection_distance));
    if (point_intersection.substract(p).magnitude() > cylinder.get_radius()) {
      return false;
    }

    double const dot_product = normal_vector.dot(direction);
    front_face_out           = (dot_product < 0);

    if (!front_face_out) {
      normal_vector.dot_in_place(-1);
    }
    return true;
  }

  bool Ray::cylinder_lower_base_intersection(Cylinder const & cylinder, bool & front_face_out) {
    Vector const a_hat = cylinder.get_edge().normalized();
    Point const p      = cylinder.get_center().substract(a_hat.dot(cylinder.get_height() / 2));
    normal_vector      = a_hat.dot(-1);
    Vector const rp    = p.substract(origin);

    if (std::abs(direction.dot(normal_vector)) < 1e-8) {
      return false;
    }

    intersection_distance = rp.dot(normal_vector) / direction.dot(normal_vector);
    if (intersection_distance < 1e-3) {
      return false;
    }

    point_intersection = origin.add(direction.dot(intersection_distance));
    if (point_intersection.substract(p).magnitude() > cylinder.get_radius()) {
      return false;
    }

    double const dot_product = normal_vector.dot(direction);
    front_face_out           = (dot_product < 0);

    if (!front_face_out) {
      normal_vector.dot_in_place(-1);
    }
    return true;
  }

  void Ray::color_contribution(Color const & dark_color, Color const & light_color,
                               std::mt19937_64 & rng, bool front_face) {
    if (intersection_distance == -1.0) {
      background_color_contribution(dark_color, light_color);
      return;
    }

    if (std::holds_alternative<Matte>(intersection_material)) {
      auto const & matte = std::get<Matte>(intersection_material);
      matte_color_contribution(rng);
      intersection_color = matte.get_reflectance();
    } else if (std::holds_alternative<Metal>(intersection_material)) {
      auto const & metal = std::get<Metal>(intersection_material);
      metal_color_contribution(metal, rng);
      intersection_color = metal.get_reflectance();
    } else if (std::holds_alternative<Refractive>(intersection_material)) {
      auto const & refractive = std::get<Refractive>(intersection_material);
      refractive_color_contribution(refractive, front_face);
      intersection_color = Color(1.0, 1.0, 1.0);
    }
  }

  void Ray::background_color_contribution(Color const & dark_color, Color const & light_color) {
    double const mix_factor = (direction.normalized().get_y() + 1.0) / 2.0;
    intersection_color =
        light_color.multiply(1.0 - mix_factor).add(dark_color.multiply(mix_factor));
  }

  void Ray::matte_color_contribution(std::mt19937_64 & rng) {
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    Vector const random_vector     = Vector(dist(rng), dist(rng), dist(rng));
    Vector const reflection_vector = normal_vector.add(random_vector);

    if (std::abs(reflection_vector.get_x()) < 1e-8 and
        std::abs(reflection_vector.get_y()) < 1e-8 and
        std::abs(reflection_vector.get_z()) < 1e-8)
    {
      reflected_direction = normal_vector;
    } else {
      reflected_direction = reflection_vector.normalized();
    }
  }

  void Ray::metal_color_contribution(Metal const & metal, std::mt19937_64 & rng) {
    Vector const initial_reflection =
        direction.substract(normal_vector.dot(2.0 * direction.dot(normal_vector)));
    std::uniform_real_distribution<double> dist(-metal.get_difusion_factor(),
                                                metal.get_difusion_factor());

    Vector const difusion_vector = Vector(dist(rng), dist(rng), dist(rng));
    reflected_direction          = initial_reflection.normalized().add(difusion_vector);
    reflected_direction          = reflected_direction.normalized();
  }

  void Ray::refractive_color_contribution(Refractive const & refractive, bool front_face) {
    Vector const u_hat = direction.normalized();
    Vector const n_hat = normal_vector;

    double const cos_theta = std::min(-u_hat.dot(n_hat), 1.0);
    double const sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    double corrected_refraction_index = refractive.get_refraction_index();

    if (front_face) {
      corrected_refraction_index = 1.0 / corrected_refraction_index;
    }

    if (corrected_refraction_index * sin_theta > 1.0) {
      reflected_direction = u_hat.substract(n_hat.dot(2.0 * u_hat.dot(n_hat)));
    } else {
      Vector const u = u_hat.add(n_hat.dot(cos_theta)).dot(corrected_refraction_index);
      double const u_magnitude_squared = u.dot(u);
      Vector const v                   = n_hat.dot(-std::sqrt(1.0 - u_magnitude_squared));
      reflected_direction              = u.add(v);
    }

    reflected_direction = reflected_direction.normalized();
  }

}  // namespace render
