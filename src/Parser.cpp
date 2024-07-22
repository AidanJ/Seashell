#include "Parser.hpp"
#include "src/Expr.hpp"
#include <algorithm>
#include <fmt/core.h>
#include <stdexcept>

[[nodiscard]] auto
Parser::receive_expressions(std::optional<std::vector<Token>> tokens
) -> std::variant<Expr::T, std::string> {
  if(tokens) {
    tokens_ = std::move(tokens.value());
    pos_ = 0;
  }

  try {
    return expression();
  } catch (std::exception const& error) {
    return fmt::format(
        "Syntax error at {}: {}", peek().display(), error.what()
    );
  }
}

[[nodiscard]] auto Parser::peek() const -> Token const& {
  if (!is_eof()) {
    return tokens_.at(pos_);
  }
  fmt::println("Unreachable peek");
}

[[nodiscard]] auto Parser::peek_last() const -> Token const& {
  if (pos_ != 0) {
    return tokens_.at(pos_ - 1);
  }
  fmt::println("Unreachable peek_last");
}

[[nodiscard]] auto Parser::is_eof() const -> bool {
  return pos_ == tokens_.size();
}

auto Parser::advance() -> void {
  if (!is_eof()) {
    ++pos_;
  }
}

[[nodiscard]] auto
Parser::match_kind(std::initializer_list<Token::Kind> const target) -> bool {
  if (is_eof()) {
    return false;
  }
  if (std::ranges::any_of(target, [this](auto const kind) {
        return !is_eof() && peek().kind_ == kind;
      })) {
    advance();
    return true;
  }

  return false;
}

[[nodiscard]] auto Parser::expression() -> Expr::T { return equality(); }

// TODO: Provide generic method for dealing with left-associative serieses of
// binary operators
// TODO: Examine performance changes by 'moving' expressions
[[nodiscard]] auto Parser::equality() -> Expr::T {
  auto left = comparison();

  while (match_kind({Token::Kind::BANG_EQUAL, Token::Kind::EQUAL_EQUAL})) {
    // implicitly copying token
    auto operation = peek_last();
    auto right = comparison();

    left = Expr::Binary::init(
        std::move(left), std::move(operation), std::move(right)
    );
  }

  return left;
}

[[nodiscard]] auto Parser::comparison() -> Expr::T {
  auto left = term();

  using Kind = Token::Kind;
  while (match_kind(
      {Kind::LESS, Kind::LESS_EQUAL, Kind::GREATER, Kind::GREATER_EQUAL}
  )) {
    auto operation = peek_last();
    auto right = term();

    left = Expr::Binary::init(
        std::move(left), std::move(operation), std::move(right)
    );
  }

  return left;
}

[[nodiscard]] auto Parser::term() -> Expr::T {
  auto left = factor();

  while (match_kind({Token::Kind::PLUS, Token::Kind::MINUS})) {
    auto operation = peek_last();
    auto right = factor();

    left = Expr::Binary::init(
        std::move(left), std::move(operation), std::move(right)
    );
  }

  return left;
}
[[nodiscard]] auto Parser::factor() -> Expr::T {
  auto left = unary();

  while (match_kind({Token::Kind::STAR, Token::Kind::SLASH})) {
    auto operation = peek_last();
    auto right = unary();

    left = Expr::Binary::init(
        std::move(left), std::move(operation), std::move(right)
    );
  }

  return left;
}
// end of left-asso

[[nodiscard]] auto Parser::unary() -> Expr::T {
  if (match_kind({Token::Kind::BANG, Token::Kind::MINUS})) {
    auto operation = peek_last();
    // can't just pass `peek_last` since order of evaluation is unknown
    return Expr::Unary::init(std::move(operation), unary());
  }
  return primary();
}

[[nodiscard]] auto Parser::primary() -> Expr::T {
  using Kind = Token::Kind;
  if (match_kind({Kind::TRUE, Kind::FALSE, Kind::STRING, Kind::NUMBER})) {
    return Expr::Literal{peek_last()};
  }
  if (match_kind({Kind::LEFT_PAREN})) {
    auto expr = expression();
    if (!match_kind({Kind::RIGHT_PAREN})) {
      throw std::logic_error("missing )");
    }
    return Expr::Grouping::init(std::move(expr));
  }
  throw std::logic_error("expected expression");
}
