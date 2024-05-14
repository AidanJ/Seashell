#include <array>
#include <climits>
#include <filesystem>
#include <istream>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <lyra/lyra.hpp>
#include <sys/wait.h>
#include <sysexits.h>
#include <unistd.h>

constexpr auto eprintln(std::string message) -> void {
  using namespace fmt;
  print("{} {}\n", format(fg(color::pale_violet_red), "[ERROR]"), message);
}

auto execute_command(std::string_view const line) -> void {
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
    [[unlikely]] eprintln("Could not fork. Computer resources might limited. "
                          "Please try again later.");
    std::exit(EX_OSERR);
    break;
  case 0:
    if (execvp(argv[0].c_str(), const_cast<char* const*>(c_argv.data())) ==
        -1) {
      eprintln("Could not execute the specified command");
    }
    break;
  default:
    waitpid(child_pid, nullptr, 0);
  }
}

auto display_prompt() -> void {
  std::array<char, HOST_NAME_MAX> hostname{0};
  gethostname(hostname.data(), sizeof(hostname) - 1);

  auto const cwd = std::filesystem::current_path();

  fmt::print("[{}@{}]$ ", hostname.data(), cwd.c_str());
}

auto main(int argc, char** argv) -> int {
  std::optional<std::string> filename{};

  auto cli_parser =
      lyra::cli() |
      lyra::opt(filename, "file").name("-f").name("--file").optional();
  auto const parse_result = cli_parser.parse({argc, argv});
  if (!parse_result) {
    eprintln(parse_result.message());
    return EX_USAGE;
  }

  display_prompt();
  for (std::string line;
       std::getline(std::cin >> std::ws, line) && line != "exit";) {
    execute_command(line);
    display_prompt();
  }
}
