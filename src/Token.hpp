#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

class Token {
public:
  // TODO Add more shell related keywords (e.g. piping)
  enum class Kind {
    // Single Character
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    COLLON,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    PERCENT,

    // One or two character
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    BEGIN,
    END,
    ELSE,
    FALSE,
    FOR,
    IF,
    IN,
    OR,
    PRINT,
    TRUE,
    LET,
    WHILE,

    Size
  };

  using Literal = std::variant<std::string, double>;

  Kind const kind_;
  uint32_t const line_;
  std::optional<Literal> const literal_;

  // TODO: Enforce literal_ argument for the relevant kinds?
  inline explicit Token(
      Kind kind, uint32_t line,
      std::optional<Literal> literal = std::nullopt
  )
      : kind_(kind), line_(line), literal_(std::move(literal)) {};

  [[nodiscard]] auto display() const -> std::string;
};
