#include <gtest/gtest.h>

#include "../common/include/vector.hpp"

namespace {

  TEST(test_vector, magnitude_zero) {
    render::Vector const vec{0.0, 0.0, 0.0};
    EXPECT_EQ(vec.magnitude(), 0.0);
  }

  TEST(test_vector, magnitude_positive) {
    render::Vector const vec{3.0, 4.0, 0.0};
    EXPECT_EQ(vec.magnitude(), 5.0);
  }

}  // namespace
