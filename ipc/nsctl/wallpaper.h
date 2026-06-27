#pragma once

#include <filesystem>
#include <qcontainerfwd.h>
#include <string_view>

namespace wpp {
std::filesystem::path evalPath(std::string_view path);

int parse_wallpaper_command(QStringList &args);

int wallpaper_move_forward();

int wallpaper_replace(QStringList &args);
} // namespace wpp
