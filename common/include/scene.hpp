#ifndef RENDER_SCENE_HPP
#define RENDER_SCENE_HPP

#include "../include/color.hpp"
#include "../include/cylinder.hpp"
#include "../include/matte.hpp"
#include "../include/metal.hpp"
#include "../include/pov.hpp"
#include "../include/ray.hpp"
#include "../include/refractive.hpp"
#include "../include/sphere.hpp"
#include "../include/vector.hpp"
#include <map>
#include <sys/types.h>
#include <utility>
#include <vector>

namespace render {

  struct Pixel {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
  };

  struct IntersectionInfo {
    double closest_distance = std::numeric_limits<double>::max();
    Point closest_point{0.0, 0.0, 0.0};
    Vector closest_normal{0.0, 0.0, 0.0};
    t_material closest_material{
      Matte{"none", Color(1.0, 1.0, 1.0)}
    };
    bool closest_front_face = true;
  };

  class Scene {
  public:
    Scene() = default;

    Scene(std::vector<Sphere> spheres, std::vector<Cylinder> cylinders, Pov pov,
          int samples_per_pixel, int max_depth, uint64_t material_rng_seed, uint64_t rays_rng_seed,
          Color background_dark_color, Color background_light_color)
        : spheres{std::move(spheres)}, cylinders{std::move(cylinders)}, pov{pov},
          samples_per_pixel{samples_per_pixel}, max_depth{max_depth},
          material_rng_seed{material_rng_seed}, rays_rng_seed{rays_rng_seed},
          background_dark_color{background_dark_color},
          background_light_color{background_light_color} { }

    // Getters

    [[nodiscard]] Pov & get_pov() { return pov; }

    [[nodiscard]] Pov const & get_pov() const { return pov; }

    [[nodiscard]] std::map<std::string, std::size_t> & get_material_index() {
      return material_index;
    }

    [[nodiscard]] int get_samples_per_pixel() const { return samples_per_pixel; }

    [[nodiscard]] int get_max_depth() const { return max_depth; }

    [[nodiscard]] std::uint64_t get_material_rng_seed() const { return material_rng_seed; }

    [[nodiscard]] std::uint64_t get_rays_rng_seed() const { return rays_rng_seed; }

    [[nodiscard]] Color const & get_background_dark_color() const { return background_dark_color; }

    [[nodiscard]] Color const & get_background_light_color() const {
      return background_light_color;
    }

    [[nodiscard]] double get_gamma() const { return gamma; }

    [[nodiscard]] std::vector<render::Matte> const & get_mattes() const { return mattes; }

    [[nodiscard]] std::vector<render::Metal> const & get_metals() const { return metals; }

    [[nodiscard]] std::vector<render::Refractive> const & get_refractives() const {
      return refractives;
    }

    [[nodiscard]] std::vector<Sphere> const & get_spheres() const { return spheres; }

    [[nodiscard]] std::vector<Cylinder> const & get_cylinders() const { return cylinders; }

    // Setters
    void set_samples_per_pixel(int s) { samples_per_pixel = s; }

    void set_max_depth(int d) { max_depth = d; }

    void set_material_rng_seed(uint64_t s) { material_rng_seed = s; }

    void set_rays_rng_seed(uint64_t s) { rays_rng_seed = s; }

    void set_background_dark_color(Color const & c) { background_dark_color = c; }

    void set_background_light_color(Color const & c) { background_light_color = c; }

    void set_gamma(double g) { gamma = g; }

    void set_pov(Pov & p) { pov = p; }

    // Raytracing

    bool test_sphere_intersections(Ray & ray, IntersectionInfo & info);
    static bool update_closest_hit(Ray const & ray, Cylinder const & cylinder,
                                   IntersectionInfo & info, bool front_face);
    bool test_cylinder_intersections(Ray & ray, IntersectionInfo & info);
    void find_closest_intersection(Ray & ray, bool & front_face_out);

    [[nodiscard]] Pixel get_pixel_color(int f, int c, std::mt19937_64 & rng,
                                        std::mt19937_64 & m_rng);

    // add

    void add_sphere(Sphere const & sphere) { spheres.push_back(sphere); }

    void add_cylinder(Cylinder const & cylinder) { cylinders.push_back(cylinder); }

    void add_pov(Pov const & p) { pov = p; }

    void add_material_matte(Matte const & matte, std::string const & line_content);

    void add_material_metal(Metal const & metal, std::string const & line_content);

    void add_material_refractive(Refractive const & refractive, std::string const & line_content);

  private:
    std::vector<Sphere> spheres;
    std::vector<Cylinder> cylinders;
    render::Pov pov;
    std::vector<Metal> metals;
    std::vector<Matte> mattes;
    std::vector<Refractive> refractives;
    std::map<std::string, std::size_t> material_index;

    int samples_per_pixel        = 20;
    int max_depth                = 5;
    uint64_t material_rng_seed   = 13;
    uint64_t rays_rng_seed       = 19;
    Color background_dark_color  = {0.25, 0.5, 1.0};
    Color background_light_color = {1.0, 1.0, 1.0};
    double gamma                 = 2.2;
  };

}  // namespace render

#endif
