#include "globals.h"

#include <cstdio>
#include <format>
#include <print>
#include <utility>

template <typename... Args>
void log(std::FILE                  *stream,
         std::format_string<Args...> fmt,
         Args &&...args) {
  if (!g_appFlags.isQuiet)
    std::println(stream, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log(std::format_string<Args...> fmt,
         Args &&...args) {
  log(stdout, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void logError(std::format_string<Args...> fmt,
              Args &&...args) {
  log(stderr, fmt, std::forward<Args>(args)...);
}
