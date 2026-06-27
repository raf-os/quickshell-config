#include "wallpaper.h"
#include "dbus.h"
#include "logging.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <filesystem>
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qobject.h>
#include <stdexcept>
#include <string_view>

namespace wpp {
namespace {
constexpr static std::array<std::string_view, 3> ALLOWED_FILE_TYPES{
    ".jpg", ".jpeg", ".png"};

bool isFileTypeValid(const std::filesystem::path &file) {
  if (file.empty())
    return false;

  return std::find(ALLOWED_FILE_TYPES.cbegin(),
                   ALLOWED_FILE_TYPES.cend(),
                   file.extension()) != ALLOWED_FILE_TYPES.cend();
}
} // namespace

std::filesystem::path evalPath(std::string_view _path) {
  if (_path.empty())
    return std::filesystem::path();

  if (_path.substr(0, 2) == "~/") {
    auto home = std::getenv("HOME");
    if (home) {
      return std::filesystem::path(home) / _path.substr(2);
    } else
      return std::filesystem::canonical(_path);
  } else {
    return std::filesystem::canonical(_path);
  }
}

int parse_wallpaper_command(QStringList &args) {
  if (args.isEmpty())
    return 1;

  const auto cmd = args.takeFirst();

  if (cmd == "next") {
    return wallpaper_move_forward();
  } else if (cmd == "set") {
    return wallpaper_replace(args);
  } else {
    return 1;
  }
}

int wallpaper_move_forward() {
  try {
    auto instance = db::getInterface();
    auto response = instance->NextWallpaper().value();
    if (response) {
      log("ok");
      return 0;
    } else {
      logError("Failed changing wallpaper.");
      return 0;
    }
  } catch (const std::runtime_error &e) {
    logError("Caught exception: {}", e.what());
    return 1;
  }
}

int wallpaper_replace(QStringList &args) {
  if (args.isEmpty())
    return 1;

  auto _p   = args.takeFirst();
  auto path = evalPath(_p.toStdString());

  if (!std::filesystem::exists(path)) {
    logError("Provided file does not exist.");
    return 1;
  }

  if (!isFileTypeValid(path)) {
    logError("Provided file format '{}' is not supported.",
             path.extension().string());
    return 1;
  }

  const auto fillMode = args.isEmpty() ? "fill" : args.takeFirst();

  try {
    auto instance = db::getInterface();
    auto response =
        instance
            ->ChangeWallpaper(QString::fromStdString(path.string()), fillMode)
            .value();
    if (response) {
      log("ok");
      return 0;
    } else {
      logError("Failed setting wallpaper.");
      return 1;
    }
  } catch (const std::runtime_error &e) {
    logError("Caught exception: {}", e.what());
    return 1;
  }
}
} // namespace wpp
