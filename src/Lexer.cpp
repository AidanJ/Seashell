#include "Lexer.hpp"
#include <cctype>
#include <locale>
#include <string>
#include <unordered_map>

#include <fmt/core.h>

// Unnamed / anonymous namespaces are preferred over globally declared variables
// which are specified as static
namespace {
  using Kind = Token::Kind;
  std::unordered_map<std::string_view, Token::Kind> keywords{
      {"&&", Kind::AND},
      {">", Kind::GREATER},
      {">=", Kind::GREATER_EQUAL},
      {"=", Kind::EQUAL},
      {"==", Kind::EQUAL_EQUAL},
      {"!", Kind::BANG},
      {"!=", Kind::BANG_EQUAL},
      {"<", Kind::LESS},
      {"<=", Kind::LESS_EQUAL},
      {"begin", Kind::BEGIN},
      {"end", Kind::END},
      {"else", Kind::ELSE},
      {"false", Kind::FALSE},
      {"for", Kind::FOR},
      {"if", Kind::IF},
      {"in", Kind::IN},
      {"||", Kind::OR},
      {"print", Kind::PRINT},
      {"true", Kind::TRUE},
      {"let", Kind::LET},
      {"while", Kind::WHILE},
  };

  std::locale const locale{"C"};
} // namespace

// skip line on comment, getting identifier name

Lexer::Lexer(std::string_view const source) : source_(source) {}

// TODO: Add '\' for a multi-line expression?
// TODO: force optional tokens input (in repl mode each line tokens get
// combined)
[[nodiscard]] auto
Lexer::receive_tokens(std::optional<std::string_view> const next_source
) -> std::vector<Token> {
  // Used for REPL Mode. Updating source_ to hold the content of the next line
  if (next_source) {
    source_ = next_source.value();
    ++line_;
  }
  // Token detection should be stateless. Therefore there shouldn't be any
  // backtracing to  inserted elements
  std::vector<Token> tokens{};

  // While a function can be used to cover tokens insertion action, I prefer not
  // to abstract away details which do not cause rewriting the same thing that
  // much:)
  while (!is_eof()) {
    if (is_whitespace(peek())) {
      advance();
      continue;
    }
    switch (peek()) {
    case '(':
      tokens.emplace_back(Token::Kind::LEFT_PAREN, line_);
      break;
    case ')':
      tokens.emplace_back(Token::Kind::RIGHT_PAREN, line_);
      break;
    case '[':
      tokens.emplace_back(Token::Kind::LEFT_BRACE, line_);
      break;
    case ']':
      tokens.emplace_back(Token::Kind::RIGHT_BRACE, line_);
      break;
    case ',':
      tokens.emplace_back(Token::Kind::COMMA, line_);
      break;
    case ':':
      tokens.emplace_back(Token::Kind::COMMA, line_);
      break;
    case '.':
      tokens.emplace_back(Token::Kind::DOT, line_);
      break;
    case '-':
      tokens.emplace_back(Token::Kind::MINUS, line_);
      break;
    case '+':
      tokens.emplace_back(Token::Kind::PLUS, line_);
      break;
    case ';':
      tokens.emplace_back(Token::Kind::SEMICOLON, line_);
      break;
    case '/':
      tokens.emplace_back(Token::Kind::SLASH, line_);
      break;
    case '*':
      tokens.emplace_back(Token::Kind::STAR, line_);
      break;
    case '"': {
      auto const string = read_string();
      tokens.emplace_back(Token::Kind::STRING, line_, std::string{string});
      break;
    }
    case '%':
      if (peek_last() == '%') {
        // NOTE: Can be optimized for REPL mode in which it would be considered
        // the end of the current source.
        skip_line();
      } else {
        tokens.emplace_back(Token::Kind::PERCENT, line_);
      }
      break;
    case '\n':
      ++line_;
      break;
    default: {
      if (std::isdigit(peek(), locale)) {
        auto const number = read_number();
        if (is_eof() || is_whitespace(peek())) {
          tokens.emplace_back(Token::Kind::NUMBER, line_, number);
        } else {
          // TODO: Syntax error
        }
        break;
      }

      auto const keyword = read_keyword();
      fmt::println("Keyword: {}", keyword);
      if (keywords.contains(keyword)) {
        tokens.emplace_back(keywords[keyword], line_);
      } else {
        tokens.emplace_back(
            Token::Kind::IDENTIFIER, line_, std::string{keyword}
        );
      }
    }
    }

    advance();
  }

  return tokens;
}

[[nodiscard]] auto Lexer::peek() const -> char {
  if (!is_eof()) {
    return source_[pos_];
  }
  return '\0';
}

[[nodiscard]] auto Lexer::peek_last() const -> char {
  if(pos_ == 0) {
    return '\0';
  }
  return source_[pos_ - 1];
}

[[nodiscard]] auto Lexer::is_eof() const -> bool {
  return pos_ == source_.size();
}

auto Lexer::advance() -> void {
  if (!is_eof()) {
    ++pos_;
  }
}

auto Lexer::skip_line() -> void {
  for (; !is_eof() && peek() != '\n'; ++pos_) {
  }
}

[[nodiscard]] auto Lexer::is_whitespace(char const let) -> bool {
  switch (let) {
  case ' ':
  case '\n':
  case '\r':
  case '\t':
  case '\f':
    return true;
  default:
    return false;
  }
}
// NOTE: returning a string_view is usually not a good idea (or any view without
// ownership) but in this case the returned type is ensured to be valid while
// source_'s resources exist
[[nodiscard]] auto Lexer::read_keyword() -> std::string_view {
  auto const begin = pos_;
  for (; !is_eof() && !is_whitespace(peek()); ++pos_) {
    if (!std::isalpha(peek(), locale)) {
      // TODO: Fatal error
    }
  }

  return source_.substr(begin, pos_ - begin);
}
[[nodiscard]] auto Lexer::read_string() -> std::string_view {
  auto const begin = pos_;

  advance();
  for (; !is_eof() && peek() != '"'; ++pos_) {
  }
  if (is_eof()) {
    // TODO: Syntax error
  }

  return source_.substr(begin + 1, pos_ - 1 - begin);
}

[[nodiscard]] auto Lexer::read_number() -> double {
  auto const begin = pos_;
  bool after_decimal_point = false;

  for (; !is_eof() && (std::isdigit(peek(), locale) ||
                       (peek() == '.' && !after_decimal_point));
       ++pos_) {
    if (peek() == '.') {
      after_decimal_point = true;
    }
  }

  return std::stod(std::string{source_.substr(begin, pos_ - begin)});
}
