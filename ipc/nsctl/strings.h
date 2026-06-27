#pragma once

#include <string_view>

// clang-format off
const std::string_view USAGE =
R"#(usage: nsctl [flags] <command> [args...|--help]

commands:
    wp (...)    Issue a wallpaper command

flags:
    --quiet, -q   Supress output messages

--help, -h <command>: Can provide help about a
                      specific command that's followed
                      by an ellipsis (...).
)#";

const std::string_view WPHELP =
R"#(usage: nsctl [flags] wp [args...]

Allows the user to request wallpaper changes. Supported file formats are jpg/jpeg and png.

commands:
    wp next   Switch to next wallpaper in the list.
              Does nothing if there's only one wallpaper.
    wp set <path> <fillMode>
              Sets the wallpaper to <path>. <fillMode>
              can be either "fit", "fill" or "stretch".
              This will override current settings.
    wp append <path> <fillMode> <duration>
              Appends the file at <path> with <fillMode>
              (see above) with <duration> at the end of
              the wallpaper list. Setting duration to -1
              means the wallpaper will not change once
              it is reached.
    wp append_dialog <path>
              Same as above, but opens up a GUI window
              on the shell to properly configure settings.
)#";
// clang-format on
