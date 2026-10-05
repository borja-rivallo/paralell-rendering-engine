#include "../../common/include/logic.hpp"
#include "../../common/include/pov.hpp"
#include "../../common/include/scene.hpp"
#include "../include/image_aos.hpp"
#include "../include/parallel_render.hpp"

#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace render;

int main(int argc, char * argv[]) {
  try {
    std::vector<std::string> arguments(argv, argv + argc);
    validate_arguments(argc, arguments);
    Scene scene = load_scene(arguments[1], arguments[2]);

    int const image_height = scene.get_pov().get_image_height();
    int const image_width  = scene.get_pov().get_image_width();
    std::size_t const total_pixels =
        static_cast<std::size_t>(image_width) * static_cast<std::size_t>(image_height);

    PixelAOS pixels_aos(total_pixels);
    std::ofstream ppm_file(arguments[3]);
    write_ppm_header(ppm_file, image_width, image_height);

    parallel_render(scene, pixels_aos, image_width, image_height);

    pixels_aos.write(ppm_file);
    ppm_file.close();
    return 0;

  } catch (std::exception const & e) {
    std::cerr << e.what() << "\n";
    return 1;
  }
}
