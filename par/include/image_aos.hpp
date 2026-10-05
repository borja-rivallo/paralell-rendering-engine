#ifndef PIXEL_AOS_HPP
#define PIXEL_AOS_HPP

#include "../../common/include/scene.hpp"
#include <ostream>
#include <vector>

namespace render {

  class PixelAOS {
  public:
    std::vector<Pixel> pixels;

    PixelAOS(size_t size) : pixels(size) { }

    void set(size_t index, Pixel const & pixel) { pixels[index] = pixel; }

    void write(std::ostream & out) const {
      for (auto const & pixel : pixels) {
        out << static_cast<int>(pixel.r) << " " << static_cast<int>(pixel.g) << " "
            << static_cast<int>(pixel.b) << "\n";
      }
    }
  };

}  // namespace render

#endif
