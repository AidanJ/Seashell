#include "Lexer.hpp"
#include <cctype>
#include <locale>
#include <string>
#include <unordered_map>

// Unnamed / anonymous namespaces are preferred over globally declared variables
// which are specified as static
namespace {
  using Kind = Token::Kind;
  std::unordered_map<std::string_view, Token::Kind> keywords{
      {"(", Kind::LEFT_PAREN},
      {")", Kind::RIGHT_PAREN},
      {"[", Kind::LEFT_BRACE},
      {"]", Kind::RIGHT_BRACE},
      {",", Kind::COMMA},
      {":", Kind::COLLON},
      {".", Kind::DOT},
      {"-", Kind::MINUS},
      {"+", Kind::PLUS},
      {";", Kind::SEMICOLON},
      {"/", Kind::SLASH},
      {"*", Kind::STAR},
      {"&&", Kind::AND},
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
    case '=':
      if(peek_next() == '=') {
        tokens.emplace_back(Token::Kind::EQUAL_EQUAL, line_);
        advance();
      } else {
        tokens.emplace_back(Token::Kind::EQUAL, line_);
      }
      break;
    case '!':
      if(peek_next() == '=') {
        tokens.emplace_back(Token::Kind::BANG_EQUAL, line_);
        advance();
      } else {
        tokens.emplace_back(Token::Kind::BANG, line_);
      }
      break;
    case '>':
      if(peek_next() == '=') {
        tokens.emplace_back(Token::Kind::GREATER_EQUAL, line_);
        advance();
      } else {
        tokens.emplace_back(Token::Kind::GREATER, line_);
      }
      break;
    case '<':
      if(peek_next() == '=') {
        tokens.emplace_back(Token::Kind::LESS_EQUAL, line_);
        advance();
      } else {
        tokens.emplace_back(Token::Kind::LESS, line_);
      }
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
        tokens.emplace_back(Token::Kind::NUMBER, line_, number);
        break;
      }

      auto const keyword = read_keyword();
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
  if (pos_ > 0) {
    return source_[pos_ - 1];
  }
  return '\0';
}

[[nodiscard]] auto Lexer::peek_next() const -> char {
  if (pos_ + 1 != source_.size()) {
    return source_[pos_ + 1];
  }
  return '\0';
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

// TODO: Allow digits in identifier's name
[[nodiscard]] auto Lexer::read_keyword() -> std::string_view {
  auto const begin = pos_;
  for (; !is_eof() && !is_whitespace(peek_next()); advance()) {
    if (!std::isalpha(peek_next(), locale)) {
      // TODO: Fatal error
      break;
    }
  }

  return source_.substr(begin, pos_ + 1 - begin);
}

// TODO: Allow single-line strings only
[[nodiscard]] auto Lexer::read_string() -> std::string_view {
  auto const begin = pos_;

  advance();
  for (; !is_eof() && peek() != '"'; advance()) {
  }
  if (is_eof()) {
    // set pos to begin
    // TODO: Syntax error
  }

  return source_.substr(begin + 1, pos_ - 1 - begin);
}

[[nodiscard]] auto Lexer::read_number() -> double {
  auto const begin = pos_;
  bool after_decimal_point = false;

  for (; !is_eof() && (std::isdigit(peek_next(), locale) ||
                       (peek_next() == '.' && !after_decimal_point));
       advance()) {
    if (peek_next() == '.') {
      after_decimal_point = true;
    }
  }

  return std::stod(std::string{source_.substr(begin, pos_ + 1 - begin)});
}
