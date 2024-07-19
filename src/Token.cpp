#include "Token.hpp"
#include <array>
#include <string_view>
#include <utility>

namespace {
  // NOTE: designated array initializers are not supported in C++ to easily
  // created a compile time mapping
  [[nodiscard]] consteval auto init_kind_map() {
    using Kind = Token::Kind;
    std::array<std::string_view, std::to_underlying(Token::Kind::Size)> map{};
    map[std::to_underlying(Token::Kind::LEFT_PAREN)] = "(";
    map[std::to_underlying(Kind::RIGHT_PAREN)] = ")";
    map[std::to_underlying(Kind::LEFT_BRACE)] = "[";
    map[std::to_underlying(Kind::RIGHT_BRACE)] = "]";
    map[std::to_underlying(Kind::COMMA)] = ",";
    map[std::to_underlying(Kind::COLLON)] = ":";
    map[std::to_underlying(Kind::DOT)] = ".";
    map[std::to_underlying(Kind::MINUS)] = "-";
    map[std::to_underlying(Kind::PLUS)] = "+";
    map[std::to_underlying(Kind::SEMICOLON)] = ";";
    map[std::to_underlying(Kind::SLASH)] = "/";
    map[std::to_underlying(Kind::STAR)] = "*";
    map[std::to_underlying(Kind::PERCENT)] = "%";
    map[std::to_underlying(Kind::BANG)] = "!";
    map[std::to_underlying(Kind::BANG_EQUAL)] = "!=";
    map[std::to_underlying(Kind::EQUAL)] = "=";
    map[std::to_underlying(Kind::EQUAL_EQUAL)] = "==";
    map[std::to_underlying(Kind::GREATER)] = ">";
    map[std::to_underlying(Kind::GREATER_EQUAL)] = ">=";
    map[std::to_underlying(Kind::LESS)] = "<";
    map[std::to_underlying(Kind::LESS_EQUAL)] = "<=";
    map[std::to_underlying(Kind::IDENTIFIER)] = "unknown identifier";
    map[std::to_underlying(Kind::STRING)] = "unknown string";
    map[std::to_underlying(Kind::NUMBER)] = "unknown number";
    map[std::to_underlying(Kind::AND)] = "&&";
    map[std::to_underlying(Kind::BEGIN)] = "begin";
    map[std::to_underlying(Kind::END)] = "end";
    map[std::to_underlying(Kind::ELSE)] = "else";
    map[std::to_underlying(Kind::FALSE)] = "false";
    map[std::to_underlying(Kind::FOR)] = "for";
    map[std::to_underlying(Kind::IF)] = "if";
    map[std::to_underlying(Kind::IN)] = "in";
    map[std::to_underlying(Kind::OR)] = "or";
    map[std::to_underlying(Kind::PRINT)] = "print";
    map[std::to_underlying(Kind::TRUE)] = "true";
    map[std::to_underlying(Kind::LET)] = "let";
    map[std::to_underlying(Kind::WHILE)] = "while";

    return map;
  }
  constinit auto KIND_MAP = init_kind_map();
} // namespace

// TODO: Might be possible to return a string_view of a static string?
[[nodiscard]] auto Token::display() const -> std::string {
  switch (kind_) {
  case (Kind::IDENTIFIER):
    if (literal_) {
      return "identifier: " + std::get<std::string>(literal_.value());
    }
    break;
  case (Kind::STRING):
    if (literal_) {
      return "string: " + std::get<std::string>(literal_.value());
    }
    break;
  case (Kind::NUMBER):
    if (literal_) {
      return "number: " + std::to_string(std::get<double>(literal_.value()));
    }
    break;
  // prevent the non-exhaustive matching warning
  default:
    break;
  }
  return std::string{KIND_MAP[std::to_underlying(kind_)]};
}
