#include "globals.h"
#include "strings.h"
#include "wallpaper.h"

#include <filesystem>
#include <print>
#include <qcommandlineoption.h>
#include <qcommandlineparser.h>
#include <qcontainerfwd.h>
#include <qobject.h>

int usage(const QString &arg = "") {
  if (arg.isEmpty()) {
    std::println("{}", USAGE);
    return 0;
  }

  if (arg == "wp") {
    std::println("{}", WPHELP);
    return 0;
  } else {
    std::println("Invalid help option.\n{}", USAGE);
    return 0;
  }
}

int main(int   argc,
         char *argv[]) {
  if (argc < 2) {
    return usage();
  }

  QStringList splitArgs;

  for (auto i = 0; i < argc; ++i) {
    splitArgs.append(argv[i]);
  }

  QCommandLineParser parser;
  QCommandLineOption helpOption({"h", "help"}, "Usage", "cmd");
  parser.addOption(helpOption);

  QCommandLineOption quietOption({"q", "quiet"}, "Silence outputs");
  parser.addOption(quietOption);

  parser.parse(splitArgs);

  if (parser.isSet(helpOption)) {
    const QString helpStr = parser.value(helpOption);
    return usage(helpStr);
  }

  if (parser.isSet(quietOption)) {
    g_appFlags.isQuiet = true;
  }

  auto positionalArguments = parser.positionalArguments();

  if (positionalArguments.size() < 2) {
    std::println("Invalid command provided.");
    return 1;
  }

  const auto cmd = positionalArguments.takeFirst();

  if (cmd == "wp") {
    return wpp::parse_wallpaper_command(positionalArguments);
  } else {
    return 0;
  }
}
