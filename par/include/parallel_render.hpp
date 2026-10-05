#ifndef PARALLEL_RENDER_HPP
#define PARALLEL_RENDER_HPP

#include "../../common/include/scene.hpp"
#include "image_aos.hpp"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

#include <oneapi/tbb/blocked_range2d.h>
#include <oneapi/tbb/enumerable_thread_specific.h>
#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/partitioner.h>
#include <oneapi/tbb/task_arena.h>

namespace render {

  struct ThreadRNG {
    std::mt19937_64 ray_rng;
    std::mt19937_64 mat_rng;
  };

  inline void parallel_render(Scene & scene, PixelAOS & pixels_aos, int image_width,
                              int image_height) {
    // Número de hilos y tamaño de grano -> CAMBIAR PARA PRUEBAS
    int const num_threads = 256;
    int const grain_size  = 1;
    // Limitación global de memoria
    tbb::global_control const global_limit(tbb::global_control::max_allowed_parallelism,
                                           static_cast<std::size_t>(num_threads));
    // Generación de vectores de semillas
    std::vector<std::uint64_t> ray_seeds(static_cast<std::size_t>(num_threads));
    std::vector<std::uint64_t> mat_seeds(static_cast<std::size_t>(num_threads));
    std::mt19937_64 master_ray_rng(scene.get_rays_rng_seed());
    std::ranges::generate(ray_seeds.begin(), ray_seeds.end(), std::ref(master_ray_rng));
    std::mt19937_64 master_mat_rng(scene.get_material_rng_seed());
    std::ranges::generate(mat_seeds.begin(), mat_seeds.end(), std::ref(master_mat_rng));

    // Privatización de generadores
    tbb::enumerable_thread_specific<ThreadRNG> thread_rngs([&]() {
      static std::atomic<std::size_t> counter{0};
      std::size_t const idx      = counter++;
      std::size_t const safe_idx = idx % static_cast<std::size_t>(num_threads);
      return ThreadRNG{std::mt19937_64(ray_seeds[safe_idx]), std::mt19937_64(mat_seeds[safe_idx])};
    });
    tbb::parallel_for(
        tbb::blocked_range2d<int>(0, image_height, grain_size, 0, image_width, grain_size),
        [&](tbb::blocked_range2d<int> const & r) {
          ThreadRNG & local_rng = thread_rngs.local();
          for (int f = r.rows().begin(); f != r.rows().end(); ++f) {
            for (int c = r.cols().begin(); c != r.cols().end(); ++c) {
              Pixel const pixel = scene.get_pixel_color(f, c, local_rng.ray_rng, local_rng.mat_rng);
              std::size_t const index =
                  static_cast<std::size_t>(f) * static_cast<std::size_t>(image_width) +
                  static_cast<std::size_t>(c);
              pixels_aos.set(index, pixel);
            }
          }
        },
        tbb::simple_partitioner());
  }

}  // namespace render
#endif
