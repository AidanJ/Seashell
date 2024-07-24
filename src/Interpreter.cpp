#include "Interpreter.hpp"
#include "Log.hpp"
#include <stdexcept>
#include <utility>

[[nodiscard]] auto Interpreter::eval(std::optional<Expr::T> line
) -> std::optional<Literal> {
  if (line) {
    expression_ = line.value();
  }
  try {
    return visit_expression(expression_);
  } catch (std::exception const& err) {
    Log::warn(err.what());
  }
  return std::nullopt;
}

[[nodiscard]] auto Interpreter::visit_expression(Expr::T const& expr
) const -> Literal {
  if (std::holds_alternative<Expr::BinaryPtr>(expr)) {
    return visit_binary(std::get<Expr::BinaryPtr>(expr));
  }
  if (std::holds_alternative<Expr::UnaryPtr>(expr)) {
    return visit_unary(std::get<Expr::UnaryPtr>(expr));
  }
  if (std::holds_alternative<Expr::GroupingPtr>(expr)) {
    return visit_grouping(std::get<Expr::GroupingPtr>(expr));
  }
  if (std::holds_alternative<Expr::LiteralPtr>(expr)) {
    return visit_literal(std::get<Expr::LiteralPtr>(expr));
  }

  throw std::logic_error("unsupported expression type");
}

// NOTE: Visiting methods are not static because let bindings and operations
// with side effects would require managing class state
[[nodiscard]] auto Interpreter::visit_binary(Expr::BinaryPtr const& expr
) const -> Literal {
  auto const left = visit_expression(expr->left);
  auto const right = visit_expression(expr->right);
  if (left.index() != right.index()) {
    throw std::logic_error("different expression types used in binary operation"
    );
  }
  switch (expr->operation.kind_) {
    using Kind = Token::Kind;
  case (Kind::MINUS):
  case (Kind::SLASH):
  case (Kind::STAR):
  case (Kind::GREATER):
  case (Kind::GREATER_EQUAL):
  case (Kind::LESS):
  case (Kind::LESS_EQUAL):
    if (!std::holds_alternative<double>(left)) {
      throw std::logic_error(fmt::format(
          "'{}' operation only avaliable for numbers", expr->operation.display()
      ));
    }
    break;

  case (Kind::PLUS):
  case (Kind::EQUAL_EQUAL):
  case (Kind::BANG_EQUAL):
    if (!std::holds_alternative<double>(left) &&
        !std::holds_alternative<std::string>(left)) {
      throw std::logic_error(fmt::format(
          "'{}' operation only avaliable for numbers or strings",
          expr->operation.display()
      ));
    }
    break;
  default:
    std::unreachable();
  }
  switch (expr->operation.kind_) {
    using Kind = Token::Kind;
  case (Kind::MINUS):
    return std::get<double>(left) - std::get<double>(right);
  case (Kind::SLASH):
    return std::get<double>(left) / std::get<double>(right);
  case (Kind::STAR):
    return std::get<double>(left) * std::get<double>(right);
  case (Kind::GREATER):
    return std::get<double>(left) > std::get<double>(right);
  case (Kind::GREATER_EQUAL):
    return std::get<double>(left) >= std::get<double>(right);
  case (Kind::LESS):
    return std::get<double>(left) < std::get<double>(right);
  case (Kind::LESS_EQUAL):
    return std::get<double>(left) <= std::get<double>(right);
  case (Kind::PLUS):
    if (std::holds_alternative<double>(left)) {
      return std::get<double>(left) + std::get<double>(right);
    }
    return std::get<std::string>(left) + std::get<std::string>(right);
  case (Kind::EQUAL_EQUAL):
    if (std::holds_alternative<double>(left)) {
      return std::get<double>(left) == std::get<double>(right);
    }
    return std::get<std::string>(left) == std::get<std::string>(right);
  case (Kind::BANG_EQUAL):
    if (std::holds_alternative<double>(left)) {
      return std::get<double>(left) != std::get<double>(right);
    }
    return std::get<std::string>(left) != std::get<std::string>(right);
  default:
    std::unreachable();
  }
}

[[nodiscard]] auto Interpreter::visit_unary(Expr::UnaryPtr const& expr
) const -> Literal {
  if (expr->operation.kind_ == Token::Kind::MINUS) {
    auto const literal = visit_expression(expr->expression);
    if (!std::holds_alternative<double>(literal)) {
      throw std::logic_error("sign negation only operates on numbers");
    }
    return -std::get<double>(literal);
  }
  // Replace with `not` keyword?
  if (expr->operation.kind_ == Token::Kind::BANG) {
    auto const literal = visit_expression(expr->expression);
    if (!std::holds_alternative<bool>(literal)) {
      throw std::logic_error("not operator only operates on booleans");
    }
    return !std::get<bool>(literal);
  }
  throw std::logic_error("invalid unary operation");
}

[[nodiscard]] auto Interpreter::visit_grouping(Expr::GroupingPtr const& expr
) const -> Literal {
  return visit_expression(expr->expression);
}

[[nodiscard]] auto Interpreter::visit_literal(Expr::LiteralPtr const& expr
) const -> Literal {
  switch (expr->token.kind_) {
    using Kind = Token::Kind;
  case Kind::FALSE:
    return false;
  case Kind::TRUE:
    return true;
  case Kind::NUMBER:
    if (!expr->token.literal_) {
      throw std::logic_error("empty number literal");
    }
    return std::get<double>(expr->token.literal_.value());
  case Kind::STRING: {
    if (!expr->token.literal_) {
      throw std::logic_error("empty string literal");
    }
    return std::get<std::string>(expr->token.literal_.value());
  }
  default:
    throw std::logic_error(
        fmt::format("invalid literal: \"{}\"", expr->token.display())
    );
  }
}
