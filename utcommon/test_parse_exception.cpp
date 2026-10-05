#include "../common/include/parse_exception.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace parse;

namespace {

  // Pruebas del constructor de ParseException y sus métodos
  std::string const TEST_LINE = "sphere 0 0 0 1.0 matte1";

  // Caso de prueba función miembro: constructor con inicialización correcta
  TEST(test_parse_exception, constructor_valid_initialization) {
    std::string const message = "Error parsing line";
    ParseException const ex(message, TEST_LINE);
    EXPECT_EQ(ex.what(), message);
    EXPECT_EQ(ex.get_line_content(), TEST_LINE);
  }

  // Caso de prueba función miembro: throw_invalid_parameters lanza excepción correctamente
  TEST(test_parse_exception, throw_invalid_parameters) {
    std::string const entity       = "sphere";
    std::string const expected_msg = "Error: Invalid sphere parameters";
    try {
      throw_invalid_parameters(entity, TEST_LINE);
    } catch (ParseException const & e) {
      // Verificar el mensaje y la línea
      EXPECT_EQ(e.what(), expected_msg);
      EXPECT_EQ(e.get_line_content(), TEST_LINE);
      return;
    } catch (...) {
      FAIL() << "Se lanzó una excepción del tipo incorrecto.";
    }
    FAIL() << "La función no lanzó ninguna excepción.";
  }

  // Caso de prueba función miembro: throw_material_not_found lanza excepción correctamente
  TEST(test_parse_exception, throw_material_not_found) {
    std::string const material_name = "matte1";
    std::string const expected_msg  = "Error: Material not found [matte1]";
    try {
      throw_material_not_found(material_name, TEST_LINE);
    } catch (ParseException const & e) {
      // Verificar el mensaje y la línea
      EXPECT_EQ(e.what(), expected_msg);
      EXPECT_EQ(e.get_line_content(), TEST_LINE);
      return;
    } catch (...) {
      FAIL() << "Se lanzó una excepción del tipo incorrecto.";
    }
    FAIL() << "La función no lanzó ninguna excepción.";
  }

  // Caso de prueba función miembro: throw_material_exists lanza excepción correctamente
  TEST(test_parse_exception, throw_material_exists) {
    std::string const material_name = "matte1";
    std::string const expected_msg  = "Error: Material with name [matte1] already exists";
    try {
      throw_material_exists(material_name, TEST_LINE);
    } catch (ParseException const & e) {
      // Verificar el mensaje y la línea
      EXPECT_EQ(e.what(), expected_msg);
      EXPECT_EQ(e.get_line_content(), TEST_LINE);
      return;
    } catch (...) {
      FAIL() << "Se lanzó una excepción del tipo incorrecto.";
    }
    FAIL() << "La función no lanzó ninguna excepción.";
  }

}  // namespace
