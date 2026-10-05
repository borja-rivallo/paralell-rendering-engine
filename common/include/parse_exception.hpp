#ifndef PARSE_EXCEPTION_HPP
#define PARSE_EXCEPTION_HPP

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace parse {

  class ParseException : public std::runtime_error {
  public:
    ParseException(std::string const & msg, std::string line)
        : std::runtime_error(msg), line_content(std::move(line)) { }

    [[nodiscard]] std::string const & get_line_content() const { return line_content; }

  private:
    std::string line_content;
  };

  // Excepción parámetros inválidos
  // [[noreturn]] para optimizar uso del compilador para las excepciones
  // inline para mejorar rendimiento evitando proceso de llamada y retorno
  [[noreturn]] inline void throw_invalid_parameters(std::string const & entity_type,
                                                    std::string const & line_content) {
    std::ostringstream oss;
    oss << "Error: Invalid " << entity_type << " parameters";
    throw ParseException(oss.str(), line_content);
  }

  // Excepción material no encontrado
  [[noreturn]] inline void throw_material_not_found(std::string const & material_name,
                                                    std::string const & line_content) {
    std::ostringstream oss;
    oss << "Error: Material not found [" << material_name << "]";
    throw ParseException(oss.str(), line_content);
  }

  // Excepción material con ese nombre ya existe
  [[noreturn]] inline void throw_material_exists(std::string const & material_name,
                                                 std::string const & line_content) {
    std::ostringstream oss;
    oss << "Error: Material with name [" << material_name << "] already exists";
    throw ParseException(oss.str(), line_content);
  }

}  // namespace parse

#endif
