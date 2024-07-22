#pragma once
#include "Expr.hpp"
#include "Token.hpp"
#include <initializer_list>
#include <optional>
#include <variant>
#include <vector>

// TODO: Consider adding `noexcept` where possible
// TODO: Take care of empty `tokens` case (just check if empty in `is_eof`?)
class Parser {
public:
  explicit inline Parser(std::vector<Token> tokens)
      : tokens_(std::move(tokens)) {}
  [[nodiscard]] auto receive_expressions(
      std::optional<std::vector<Token>> tokens = std::nullopt
  ) -> std::variant<Expr::T, std::string>;

private:
  std::vector<Token> tokens_;
  size_t pos_ = 0;

  [[nodiscard]] auto peek() const -> Token const&;
  [[nodiscard]] auto peek_last() const -> Token const&;
  [[nodiscard]] auto is_eof() const -> bool;

  auto advance() -> void;

  [[nodiscard]] auto match_kind(std::initializer_list<Token::Kind> target
  ) -> bool;

  // sorted by precedence level
  [[nodiscard]] auto expression() -> Expr::T;
  [[nodiscard]] auto equality() -> Expr::T;
  [[nodiscard]] auto comparison() -> Expr::T;
  [[nodiscard]] auto term() -> Expr::T;
  [[nodiscard]] auto factor() -> Expr::T;
  [[nodiscard]] auto unary() -> Expr::T;
  [[nodiscard]] auto primary() -> Expr::T;
};
