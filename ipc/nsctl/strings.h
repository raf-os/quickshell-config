#pragma once

#include <string_view>

// clang-format off
const std::string_view USAGE =
R"#(usage: nsctl [flags] <command> [args...|--help]

commands:
    wallpaper, wp (...) Issue a wallpaper command

flags:
    --quiet, -q   Supress output messages

--help, -h <command>: Can provide help about a specific command that's followed by an ellipsis (...).
)#";
// clang-format on
