#ifndef PARSE_UTIL_HPP
#define PARSE_UTIL_HPP

#include "../include/parse_exception.hpp"
#include <array>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace parse::util {

  // TODAS las funciones inline (son helpers pequeños)

  inline std::string trim(std::string const & s) {
    size_t i = 0, j = s.size();
    while (i < j and std::isspace(static_cast<unsigned char>(s[i])) != 0) {
      ++i;
    }
    while (j > i and std::isspace(static_cast<unsigned char>(s[j - 1])) != 0) {
      --j;
    }
    return s.substr(i, j - i);
  }

  inline std::vector<std::string> split_ws(std::string const & s) {
    std::istringstream iss(s);
    std::vector<std::string> out;
    std::string tok;
    while (iss >> tok) {
      out.push_back(tok);
    }
    return out;
  }

  inline double to_double_config(std::string const & s, std::string const & lineforprint,
                                 char const * where) {
    try {
      return std::stod(s);
    } catch (...) {
      std::ostringstream oss;
      oss << "Invalid" << " " << where << " parameters" << "\n"
          << "Line: \"" << lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
  }

  inline void expect_token_count(std::vector<std::string> const & tokens, size_t expected_count,
                                 std::string const & line_content,
                                 std::string const & entity_type) {
    if (tokens.size() != expected_count) {
      throw_invalid_parameters(entity_type, line_content);
    }
  }

  inline void validate_rgb(std::array<double, 3> const & color, std::string const & lineforprint,
                           std::string const & where) {
    auto in01 = [](double x) { return x >= 0.0 and x <= 1.0; };
    if (!in01(color[0]) or !in01(color[1]) or !in01(color[2])) {
      std::ostringstream oss;
      oss << "Invalid" << " " << where << " parameters" << "\n"
          << "Line: \"" << lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
  }

  inline void validate_rgb_config(std::array<double, 3> const & colors,
                                  std::string const & lineforprint, std::string_view key) {
    auto in01 = [](double v) { return v >= 0.0 and v <= 1.0; };
    if (!in01(colors[0]) or !in01(colors[1]) or !in01(colors[2])) {
      std::ostringstream oss;
      oss << "Invalid value for key: \"" << "[" << key << ":" << "]\"" << "\n"
          << "Line: \"" << lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
  }

  inline void validate_axis_nonzero(std::array<double, 3> const & axis,
                                    std::string const & lineforprint, std::string const & where) {
    if (axis[0] == 0.0 and axis[1] == 0.0 and axis[2] == 0.0) {
      std::ostringstream oss;
      oss << "Invalid" << " " << where << " parameters" << "\n"
          << "Line: \"" << lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
  }

  inline int to_int(std::string const & s, std::string const & lineforprint, char const * what) {
    try {
      return std::stoi(s);
    } catch (...) {
      std::ostringstream oss;
      oss << "Invalid value for key:" << " " << "[" << what << "]\n"
          << "Line: \"" << lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
  }

  inline void parse_three_doubles(std::string const & val, std::array<double, 3> & out,
                                  std::string const & lineforprint, char const * what) {
    std::istringstream iss(val);
    if (!(iss >> out[0] >> out[1] >> out[2])) {
      std::ostringstream oss;
      oss << "Invalid" << " " << what << " parameters" << "\n"
          << "Line: \"" << lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
  }

  inline void expect_positive(int v, std::string const & lineforprint, char const * what) {
    if (v <= 0) {
      std::ostringstream oss;
      oss << what << " must be positive (integer), got: " << v << "\n"
          << "Line: \"" << lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
  }

  inline std::string strip_comment_and_trim(std::string line) {
    if (auto pos = line.find('#'); pos != std::string::npos) {
      line = line.erase(pos);
    }
    return trim(line);
  }

  inline uint64_t to_uint64(std::string const & s, std::string const & lineforprint,
                            char const * what) {
    try {
      return static_cast<uint64_t>(std::stoull(s));
    } catch (...) {
      std::ostringstream oss;
      oss << "Cannot be converted to uint64 (" << what << "): \"" << s << "\"\n"
          << "Line: \"" << lineforprint << "\"";
      throw std::runtime_error(oss.str());
    }
  }

  // Mover to_double del .cpp al .hpp
  inline double to_double(std::string const & token) {
    try {
      size_t pos         = 0;
      double const value = std::stod(token, &pos);
      return value;
    } catch (std::exception const & e) {
      throw std::runtime_error("Conversion error: not a valid double");
    }
  }

}  // namespace parse::util

#endif
