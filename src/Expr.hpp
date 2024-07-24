#pragma once
#include <fmt/core.h>
#include <memory>
#include <variant>

#include "Token.hpp"

template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};

template <class... Ts> overloads(Ts...) -> overloads<Ts...>;

// NOTE: Recursive types have to be allocated on the heap since they can
// represent an infinite amount of expressions
namespace Expr {
  struct Literal;
  struct Grouping;
  struct Unary;
  struct Binary;

  // Might be a good idea forcing explicit `make_shared` calls instead of hiding
  // heap allocation inside the expression interfaces
  using LiteralPtr = std::shared_ptr<Literal>;
  using GroupingPtr = std::shared_ptr<Grouping>;
  using UnaryPtr = std::shared_ptr<Unary>;
  using BinaryPtr = std::shared_ptr<Binary>;

  // TODO: Move Ptr types to `detail`?
  using T = std::variant<LiteralPtr, BinaryPtr, UnaryPtr, GroupingPtr>;

  // TODO: Make Literal consistent with the others? or should terminal
  // expressions not have `init` as an initialization option
  struct Literal {
    Token const token;

    static inline auto init(Token token) -> std::shared_ptr<Literal> {
      // TODO: Check if copy constructed by using aggregate initialization
      return std::make_shared<Literal>(std::move(token));
    }
  };

  struct Grouping {
    // `detail::Grouping` can just be referred as `Grouping` but might cause
    // misunderstanding
    T expression;
    static inline auto init(T expression) -> std::shared_ptr<Grouping> {
      // TODO: Check if copy constructed by using aggregate initialization
      return std::make_shared<Grouping>(
          Grouping{.expression = std::move(expression)}
      );
    }
  };

  struct Unary {
    Token const operation;
    T expression;

    static inline auto
    init(Token operation, T expression) -> std::shared_ptr<Unary> {
      return std::make_shared<Unary>(Unary{
          .operation = std::move(operation), .expression = std::move(expression)
      });
    }
  };

  struct Binary {
    T left;
    Token const operation;
    T right;

    static inline auto
    init(T left, Token operation, T right) -> std::shared_ptr<Binary> {
      return std::make_shared<Binary>(Binary{
          .left = std::move(left),
          .operation = std::move(operation),
          .right = std::move(right)
      });
    }
  };

  inline auto display(T const& expression) -> std::string {
    return std::visit(
        overloads{
            [](LiteralPtr const& expr) { return expr->token.display(); },
            [](GroupingPtr const& expr) -> std::string {
              return fmt::format("({})", display(expr->expression));
            },
            [](UnaryPtr const& expr) -> std::string {
              return fmt::format(
                  "({} {})", expr->operation.display(),
                  display(expr->expression)
              );
            },
            [](BinaryPtr const& expr) -> std::string {
              return fmt::format(
                  "({} {} {})", expr->operation.display(), display(expr->left),
                  display(expr->right)
              );
            }
        },
        expression
    );
  };

} // namespace Expr
