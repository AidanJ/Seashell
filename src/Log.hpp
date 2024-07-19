#pragma once

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>

// TODO: Verbosity settings
// TODO: Inline reporting in REPL mode
namespace Log {
  using namespace fmt;
  inline constexpr auto error(std::string message) -> void {
    print("{} {}\n", format(fg(color::pale_violet_red), "[ERROR]"), message);
  }

  inline constexpr auto warn(std::string message) -> void {
    print("{} {}\n", format(fg(color::yellow), "[WARNING]"), message);
  }

  inline constexpr auto info(std::string message) -> void {
    print("{} {}\n", format(fg(color::sky_blue), "[INFO]"), message);
  }
}
