#pragma once
#include "Log.hpp"

#include <algorithm>
#include <array>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include <sys/wait.h>
#include <sysexits.h>
#include <unistd.h>

namespace Command {
  auto execute(std::string_view const line) -> void {
    static constexpr auto MAX_ARGS = 1 << 8;

    // Arguments ownership holder
    std::vector<std::string> argv{};
    std::array<char const*, MAX_ARGS> c_argv{0};

    std::ranges::for_each(
        std::ranges::views::split(line, ' '),
        [&argv](auto const& arg) {
          argv.emplace_back(std::string{arg.begin(), arg.end()});
        }
    );

    for (auto i = 0U; i < argv.size(); ++i) {
      c_argv[i] = argv[i].c_str();
    }

    auto const child_pid = fork();

    switch (child_pid) {
    case -1:
      [[unlikely]] Log::error(
          "Could not fork. Computer resources might limited. "
          "Please try again later."
      );
      std::exit(EX_OSERR);
      break;
    case 0:
      if (execvp(argv[0].c_str(), const_cast<char* const*>(c_argv.data())) ==
          -1) {
        Log::error("Could not execute the specified command");
      }
      break;
    default:
      waitpid(child_pid, nullptr, 0);
    }
  }
} // namespace Command
