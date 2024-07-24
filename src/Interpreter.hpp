#pragma once
#include "Expr.hpp"
#include <optional>
#include <utility>

class Interpreter {
public:
  using Literal = std::variant<std::string, double, bool>;
  inline explicit Interpreter(Expr::T expression)
      : expression_(std::move(expression)) {}
  [[nodiscard]] auto eval(
      std::optional<Expr::T> line = std::nullopt
  ) -> std::optional<Literal>;

private:
  Expr::T expression_;

  [[nodiscard]] auto visit_expression(Expr::T const& expr) const -> Literal;
  [[nodiscard]] auto visit_binary(Expr::BinaryPtr const& expr) const -> Literal;
  [[nodiscard]] auto visit_unary(Expr::UnaryPtr const& expr) const -> Literal;
  [[nodiscard]] auto visit_grouping(Expr::GroupingPtr const& expr) const -> Literal;
  [[nodiscard]] auto visit_literal(Expr::LiteralPtr const& expr) const -> Literal;
};
