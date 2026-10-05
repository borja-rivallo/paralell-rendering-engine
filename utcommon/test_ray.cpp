#include <gtest/gtest.h>

#include "../common/include/color.hpp"
#include "../common/include/cylinder.hpp"
#include "../common/include/matte.hpp"
#include "../common/include/metal.hpp"
#include "../common/include/point.hpp"
#include "../common/include/ray.hpp"
#include "../common/include/refractive.hpp"
#include "../common/include/sphere.hpp"
#include "../common/include/vector.hpp"

#include <cmath>
#include <random>

namespace {

  // ===== Helpers: ajusta si tus constructores difieren =====

  render::Matte create_default_matte() {
    return {"default", render::Color(1.0, 1.0, 1.0)};
  }

  render::Metal create_metal(char const * name, render::Color const refl, double diff) {
    return {name, refl, diff};
  }

  render::Refractive create_refr(char const * name, double index) {
    return {name, index};
  }

  // Sphere(Point, double, t_material)
  render::Sphere makeSphere(render::Point const c, double r) {
    return {c, r, render::t_material{create_default_matte()}};
  }

  // Cylinder(Point, double, Vector, t_material)
  render::Cylinder makeCylinder(render::Point const c, double r,
                                render::Vector const axis) {  // c, axis son const
    return {c, r, axis, render::t_material{create_default_matte()}};
  }

  // Comparaciones con tolerancia
  void expect_vec_near(render::Vector const & a, render::Vector const & b, double eps = 1e-12) {
    EXPECT_NEAR(a.get_x(), b.get_x(), eps);
    EXPECT_NEAR(a.get_y(), b.get_y(), eps);
    EXPECT_NEAR(a.get_z(), b.get_z(), eps);
  }

  void expect_pt_near(render::Point const & a, render::Point const & b, double eps = 1e-12) {
    EXPECT_NEAR(a.get_x(), b.get_x(), eps);
    EXPECT_NEAR(a.get_y(), b.get_y(), eps);
    EXPECT_NEAR(a.get_z(), b.get_z(), eps);
  }

  void expect_color_near(render::Color const & a, render::Color const & b, double eps = 1e-12) {
    EXPECT_NEAR(a.get_r(), b.get_r(), eps);
    EXPECT_NEAR(a.get_g(), b.get_g(), eps);
    EXPECT_NEAR(a.get_b(), b.get_b(), eps);
  }

  double vlen(render::Vector const & v) {
    return std::sqrt(v.get_x() * v.get_x() + v.get_y() * v.get_y() + v.get_z() * v.get_z());
  }

  // ===== Tests =====

  TEST(test_ray, constructor_and_getters) {
    render::Point const o(1.0, -2.0, 3.0);
    render::Vector const d(0.25, 0.5, -1.0);
    render::Ray const r{o, d, render::Color(0.0, 0.0, 0.0)};

    expect_pt_near(r.get_origin(), o);
    expect_vec_near(r.get_direction(), d);
    // Inicialmente no hay intersección
    EXPECT_DOUBLE_EQ(r.get_intersection_distance(), -1.0);
  }

  TEST(test_ray_sphere, hit_frontface_near_solution) {
    // Esfera unidad en el origen. Rayo desde z=-3 hacia +z: debe entrar en z=-1 (t=2)
    render::Ray ray{render::Point(0, 0, -3), render::Vector(0, 0, 1), render::Color(0, 0, 0)};
    render::Sphere const s = makeSphere(render::Point(0, 0, 0), 1.0);

    bool front = false;
    ASSERT_TRUE(ray.sphere_intersection(s, front));
    EXPECT_TRUE(front);
    // normal·dir < 0
    EXPECT_NEAR(ray.get_intersection_distance(), 2.0, 1e-9);
    expect_pt_near(ray.get_point_intersection(), render::Point(0, 0, -1));
    expect_vec_near(ray.get_normal_vector(), render::Vector(0, 0, -1));
    // (I-C)/r
  }

  TEST(test_ray_sphere, miss) {
    render::Ray ray{render::Point(0, 0, -3), render::Vector(0, 1, 0), render::Color(0, 0, 0)};
    render::Sphere const s = makeSphere(render::Point(0, 0, 0), 1.0);
    bool front             = false;
    EXPECT_FALSE(ray.sphere_intersection(s, front));
  }

  TEST(test_ray_cylinder_side, hit_inside_height) {
    // Cilindro: centro (0,0,0), eje Z, r=1, h=2 (axis=(0,0,2))
    render::Cylinder const cyl = makeCylinder(render::Point(0, 0, 0), 1.0, render::Vector(0, 0, 2));

    // Rayo desde x=2 hacia -x: intersección en (1,0,0), z=0 dentro de |z|<=1
    render::Ray ray{render::Point(2, 0, 0), render::Vector(-1, 0, 0), render::Color(0, 0, 0)};
    bool front = false;

    ASSERT_TRUE(ray.cylinder_side_intersection(cyl, front));
    EXPECT_TRUE(front);
    EXPECT_NEAR(ray.get_intersection_distance(), 1.0, 1e-9);
    expect_pt_near(ray.get_point_intersection(), render::Point(1, 0, 0));
    expect_vec_near(ray.get_normal_vector(), render::Vector(1, 0, 0));
  }

  TEST(test_ray_cylinder_side, miss_outside_height) {
    render::Cylinder const cyl = makeCylinder(render::Point(0, 0, 0), 1.0, render::Vector(0, 0, 2));
    // h=2

    render::Ray ray{render::Point(-2, 0, 2), render::Vector(1, 0, 0), render::Color(0, 0, 0)};
    bool front = false;
    EXPECT_FALSE(ray.cylinder_side_intersection(cyl, front));
  }

  TEST(test_ray_cylinder_bases, upper_base_hit) {
    render::Cylinder const cyl = makeCylinder(render::Point(0, 0, 0), 1.0, render::Vector(0, 0, 2));
    // base sup z=+1
    render::Ray ray{render::Point(0, 0, 3), render::Vector(0, 0, -1), render::Color(0, 0, 0)};
    bool front = false;

    ASSERT_TRUE(ray.cylinder_upper_base_intersection(cyl, front));
    EXPECT_TRUE(front);
    EXPECT_NEAR(ray.get_intersection_distance(), 2.0, 1e-9);
    expect_pt_near(ray.get_point_intersection(), render::Point(0, 0, 1));
    expect_vec_near(ray.get_normal_vector(), render::Vector(0, 0, 1));
  }

  TEST(test_ray_cylinder_bases, lower_base_hit) {
    render::Cylinder const cyl = makeCylinder(render::Point(0, 0, 0), 1.0, render::Vector(0, 0, 2));
    // base inf z=-1
    render::Ray ray{render::Point(0, 0, -3), render::Vector(0, 0, 1), render::Color(0, 0, 0)};
    bool front = false;

    ASSERT_TRUE(ray.cylinder_lower_base_intersection(cyl, front));
    EXPECT_TRUE(front);
    EXPECT_NEAR(ray.get_intersection_distance(), 2.0, 1e-9);
    expect_pt_near(ray.get_point_intersection(), render::Point(0, 0, -1));
    expect_vec_near(ray.get_normal_vector(), render::Vector(0, 0, -1));
  }

  TEST(test_ray_background, mixes_endpoints) {
    {
      render::Ray ray{render::Point(0, 0, 0), render::Vector(0, 1, 0), render::Color(0, 0, 0)};
      std::mt19937_64 rng{1'337};
      ray.set_intersection_distance(-1.0);
      // fuerza fondo
      render::Color const dark_color(0.1, 0.2, 0.3);
      render::Color const light_color(0.9, 0.8, 0.7);
      ray.color_contribution(dark_color, light_color, rng, true);
      expect_color_near(ray.get_intersection_color(),
                        dark_color);  // Vector(0,1,0) -> y=1, m=1, dark
    }

    {
      render::Ray ray{render::Point(0, 0, 0), render::Vector(0, -1, 0), render::Color(0, 0, 0)};
      std::mt19937_64 rng{1'337};
      ray.set_intersection_distance(-1.0);
      render::Color const dark_color(0.1, 0.2, 0.3);
      render::Color const light_color(0.9, 0.8, 0.7);
      ray.color_contribution(dark_color, light_color, rng, true);
      expect_color_near(ray.get_intersection_color(),
                        light_color);  // Vector(0,-1,0) -> y=-1, m=0, light
    }
  }

  TEST(test_ray_matte, reflected_unit_and_color_is_reflectance) {
    render::Ray ray{render::Point(0, 0, 0), render::Vector(0, 0, 1), render::Color(0, 0, 0)};
    ray.set_intersection_distance(1.0);
    ray.set_normal_vector(render::Vector(0, 0, 1));
    ray.set_intersection_material(render::t_material{create_default_matte()});

    std::mt19937_64 rng{45};
    render::Color const black_color(0, 0, 0);
    ray.color_contribution(black_color, black_color, rng, true);

    EXPECT_NEAR(vlen(ray.get_reflected_direction()), 1.0, 1e-12);
    // El color de intersección = reflectancia del mate (1,1,1) en create_default_matte
    expect_color_near(ray.get_intersection_color(), render::Color(1, 1, 1));
  }

  TEST(test_ray_metal, diffusion_zero_perfect_reflection) {
    render::Ray ray{render::Point(0, 0, 0), render::Vector(0, -1, 0), render::Color(0, 0, 0)};
    ray.set_intersection_distance(1.0);
    ray.set_normal_vector(render::Vector(0, 1, 0));
    render::Color const metal_color(0.8, 0.7, 0.6);
    ray.set_intersection_material(render::t_material{create_metal("steel", metal_color, 0.0)});

    std::mt19937_64 rng{1'337};
    render::Color const black_color(0, 0, 0);
    ray.color_contribution(black_color, black_color, rng, true);

    expect_vec_near(ray.get_reflected_direction(), render::Vector(0, 1, 0));
    expect_color_near(ray.get_intersection_color(), metal_color);
  }

  TEST(test_ray_refractive, total_internal_reflection_branch) {
    render::Ray ray{render::Point(0, 0, 0), render::Vector(1, 0, 0), render::Color(0, 0, 0)};
    ray.set_intersection_distance(1.0);
    ray.set_normal_vector(render::Vector(0, 0, 1));
    render::t_material const refractive_mat = render::t_material{create_refr("glass", 1.5)};
    ray.set_intersection_material(refractive_mat);

    std::mt19937_64 rng{999};
    render::Color const black_color(0, 0, 0);
    ray.color_contribution(black_color, black_color, rng, /*front_face*/ false);

    expect_vec_near(ray.get_reflected_direction(), render::Vector(1, 0, 0));
    expect_color_near(ray.get_intersection_color(), render::Color(1, 1, 1));
  }

  TEST(test_ray_refractive, refraction_normal_incidence_air_to_glass) {
    render::Ray ray{render::Point(0, 0, 0), render::Vector(0, 0, -1), render::Color(0, 0, 0)};
    ray.set_intersection_distance(1.0);
    ray.set_normal_vector(render::Vector(0, 0, 1));
    render::t_material const refractive_mat = render::t_material{create_refr("glass", 1.5)};
    ray.set_intersection_material(refractive_mat);

    std::mt19937_64 rng{42};
    render::Color const black_color(0, 0, 0);
    ray.color_contribution(black_color, black_color, rng, true);

    expect_vec_near(ray.get_reflected_direction(), render::Vector(0, 0, -1));
    expect_color_near(ray.get_intersection_color(), render::Color(1, 1, 1));
  }

}  // namespace
