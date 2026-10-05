#include "../include/parse_config.hpp"
#include "../include/color.hpp"
#include "../include/point.hpp"
#include "../include/pov.hpp"
#include "../include/scene.hpp"
#include "../include/util.hpp"
#include "../include/vector.hpp"

#include <array>
#include <cctype>
#include <cstddef>
#include <istream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

using parse::util::expect_positive;
using parse::util::parse_three_doubles;
using parse::util::strip_comment_and_trim;
using parse::util::to_double_config;
using parse::util::to_int;
using parse::util::to_uint64;
using parse::util::validate_rgb_config;

namespace {

  // ------------------- Utilidades de parsing -------------------

  inline void ensure_token_count_exact(std::string_view val, std::size_t expected,
                                       std::string const & lineforprint, std::string const & key) {
    std::string s{val};
    size_t i = 0, n = s.size();
    auto is_space = [](char c) -> bool { return std::isspace(static_cast<unsigned char>(c)) != 0; };

    std::size_t tokens = 0;
    while (i < n and tokens < expected) {
      while (i < n and is_space(s[i])) {
        ++i;
      }
      if (i >= n) {
        break;
      }
      while (i < n and !is_space(s[i])) {
        ++i;
      }
      ++tokens;
    }

    if (tokens < expected) {
      std::ostringstream oss;
      oss << "Invalid value for key: " << "[" << key << "]" << "\n"
          << "Line: \"" << lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }

    size_t j = i;
    while (j < n and is_space(s[j])) {
      ++j;
    }
    if (j < n) {
      std::string const extra = s.substr(j);
      std::ostringstream oss;
      oss << "Extra data after configuration value for key: " << "[" << key << "]" << "\n"
          << "Extra: \"" << extra << "\"\n";
      throw std::runtime_error(oss.str());
    }
  }

  parse::KeyVal split_config_line(std::string const & raw, std::regex const & re) {
    std::smatch m;
    if (!std::regex_match(raw, m, re)) {
      std::ostringstream oss;
      oss << "Invalid configuration line format." << "\n"
          << "Line: \"" << raw << "\"";
      throw std::runtime_error(oss.str());
    }
    parse::KeyVal kv;
    kv.key          = m[1].str();
    kv.val          = m[2].str();
    kv.lineforprint = raw;
    return kv;
  }

  // ------------------- Handlers (≤4 args) -------------------

  bool handle_aspect_ratio(parse::KeyVal const & kv, parse::ImageCfg & img) {
    if (kv.key != "aspect_ratio") {
      return false;
    }
    ensure_token_count_exact(kv.val, 2, kv.lineforprint, "aspect_ratio");
    std::istringstream iss{kv.val};
    int w = 16, h = 9;
    if (!(iss >> w >> h) or w <= 0 or h <= 0) {
      std::ostringstream oss;
      oss << "Invalid value for key: " << "[" << kv.key << ":" << "]" << "\n"
          << "Line: \"" << kv.lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
    img.ar_w         = w;
    img.ar_h         = h;
    img.needs_update = true;
    return true;
  }

  bool handle_image_width(parse::KeyVal const & kv, parse::ImageCfg & img) {
    if (kv.key != "image_width") {
      return false;
    }
    ensure_token_count_exact(kv.val, 1, kv.lineforprint, "image_width");
    int const w = to_int(kv.val, kv.lineforprint, "image_width");
    expect_positive(w, kv.lineforprint, "image_width");
    img.width        = w;
    img.needs_update = true;
    return true;
  }

  bool handle_camera_position(parse::KeyVal const & kv, render::Pov & pov) {
    if (kv.key != "camera_position") {
      return false;
    }
    ensure_token_count_exact(kv.val, 3, kv.lineforprint, "camera_position");
    std::array<double, 3> v{};
    parse_three_doubles(kv.val, v, kv.lineforprint, "camera_position");
    pov.set_camera_position(render::Point(v[0], v[1], v[2]));
    return true;
  }

  bool handle_camera_target(parse::KeyVal const & kv, render::Pov & pov) {
    if (kv.key != "camera_target") {
      return false;
    }
    ensure_token_count_exact(kv.val, 3, kv.lineforprint, "camera_target");
    std::array<double, 3> v{};
    parse_three_doubles(kv.val, v, kv.lineforprint, "camera_target");
    pov.set_camera_target(render::Point(v[0], v[1], v[2]));
    return true;
  }

  bool handle_camera_north(parse::KeyVal const & kv, render::Pov & pov) {
    if (kv.key != "camera_north") {
      return false;
    }
    ensure_token_count_exact(kv.val, 3, kv.lineforprint, "camera_north");
    std::array<double, 3> v{};
    parse_three_doubles(kv.val, v, kv.lineforprint, "camera_north");
    pov.set_camera_north(render::Vector(v[0], v[1], v[2]));
    return true;
  }

  bool handle_fov(parse::KeyVal const & kv, render::Pov & pov) {
    if (kv.key != "field_of_view") {
      return false;
    }
    ensure_token_count_exact(kv.val, 1, kv.lineforprint, "field_of_view");
    double const fov = to_double_config(kv.val, kv.lineforprint, "field_of_view");
    if (fov <= 0.0 or fov >= 180.0) {
      std::ostringstream oss;
      oss << "Invalid value for key: " << "[" << kv.key << ":" << "]" << "\n"
          << "Line: \"" << kv.lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
    pov.set_field_of_view(fov);
    return true;
  }

  bool handle_render(parse::KeyVal const & kv, render::Scene & scene) {
    if (kv.key == "samples_per_pixel") {
      ensure_token_count_exact(kv.val, 1, kv.lineforprint, "samples_per_pixel");
      int const v = to_int(kv.val, kv.lineforprint, "samples_per_pixel");
      expect_positive(v, kv.lineforprint, "samples_per_pixel");
      scene.set_samples_per_pixel(v);
      return true;
    }
    if (kv.key == "max_depth") {
      ensure_token_count_exact(kv.val, 1, kv.lineforprint, "max_depth");
      int const v = to_int(kv.val, kv.lineforprint, "max_depth");
      expect_positive(v, kv.lineforprint, "max_depth");
      scene.set_max_depth(v);
      return true;
    }
    if (kv.key == "gamma") {
      ensure_token_count_exact(kv.val, 1, kv.lineforprint, "gamma");
      double const g = to_double_config(kv.val, kv.lineforprint, "gamma");
      if (g <= 0.0) {
        std::ostringstream oss;
        oss << "Invalid value for key: " << "[" << kv.key << ":" << "]" << "\n"
            << "Line: \"" << kv.lineforprint << "\"";
        throw std::runtime_error(oss.str());
      }
      scene.set_gamma(g);
      return true;
    }
    return false;
  }

  bool handle_background(parse::KeyVal const & kv, render::Scene & scene) {
    std::array<double, 3> c{};
    if (kv.key == "background_dark_color") {
      ensure_token_count_exact(kv.val, 3, kv.lineforprint, "background_dark_color");
      parse_three_doubles(kv.val, c, kv.lineforprint, "background_dark");
      validate_rgb_config(c, kv.lineforprint, kv.key);
      scene.set_background_dark_color(render::Color(c[0], c[1], c[2]));
      return true;
    }
    if (kv.key == "background_light_color") {
      ensure_token_count_exact(kv.val, 3, kv.lineforprint, "background_light_color");
      parse_three_doubles(kv.val, c, kv.lineforprint, "background_light_color");
      validate_rgb_config(c, kv.lineforprint, kv.key);
      scene.set_background_light_color(render::Color(c[0], c[1], c[2]));
      return true;
    }
    return false;
  }

  bool handle_seeds(parse::KeyVal const & kv, render::Scene & scene) {
    if (kv.key == "material_rng_seed") {
      ensure_token_count_exact(kv.val, 1, kv.lineforprint, "material_rng_seed");
      scene.set_material_rng_seed(to_uint64(kv.val, kv.lineforprint, "material_rng_seed"));
      return true;
    }
    if (kv.key == "ray_rng_seed") {
      ensure_token_count_exact(kv.val, 1, kv.lineforprint, "ray_rng_seed");
      scene.set_rays_rng_seed(to_uint64(kv.val, kv.lineforprint, "ray_rng_seed"));
      return true;
    }
    return false;
  }

}  // namespace

namespace parse {

  void parse_config_stream(std::istream & in, render::Scene & scene) {
    std::regex const re_line(R"(^\s*([A-Za-z_]+):\s*(.*?)\s*$)");
    std::string raw;
    render::Pov pov;
    ImageCfg img;

    while (std::getline(in, raw)) {
      std::string const & lineforprint = raw;
      std::string const line           = strip_comment_and_trim(raw);
      if (line.empty()) {
        continue;
      }

      KeyVal kv       = split_config_line(line, re_line);
      kv.lineforprint = lineforprint;

      bool handled = false;
      handled      = handle_aspect_ratio(kv, img) or handled;
      handled      = handle_image_width(kv, img) or handled;
      handled      = handle_camera_position(kv, pov) or handled;
      handled      = handle_camera_target(kv, pov) or handled;
      handled      = handle_camera_north(kv, pov) or handled;
      handled      = handle_fov(kv, pov) or handled;
      handled      = handle_render(kv, scene) or handled;
      handled      = handle_background(kv, scene) or handled;
      handled      = handle_seeds(kv, scene) or handled;

      if (!handled) {
        std::ostringstream oss;
        oss << "Unknown configuration key: " << "[" << kv.key << ":" << "]" << "\n";
        throw std::runtime_error(oss.str());
      }
    }

    if (img.needs_update) {
      render::ImageSize const isz = render::Pov::compute_image_size(img.width, img.ar_w, img.ar_h);
      pov.set_image_size(isz);
    }
    scene.set_pov(pov);
  }

}  // namespace parse
