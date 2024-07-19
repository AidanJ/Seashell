#pragma once
#include "Token.hpp"

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

class Lexer {
public:
  explicit Lexer(std::string_view source = "");
  [[nodiscard]] auto receive_tokens(
      std::optional<std::string_view> next_source = std::nullopt
  ) -> std::vector<Token>;

private:
  size_t pos_ = 0;
  uint32_t line_ = 1;
  std::string_view source_;

  [[nodiscard]] auto peek() const -> char;
  [[nodiscard]] auto peek_last() const -> char;
  [[nodiscard]] auto is_eof() const -> bool;

  auto advance() -> void;
  auto skip_line() -> void;

  [[nodiscard]] auto read_keyword() -> std::string_view;
  [[nodiscard]] auto read_string() -> std::string_view;
  [[nodiscard]] auto read_number() -> double;

  [[nodiscard]] static auto is_whitespace(char let) -> bool;
};
