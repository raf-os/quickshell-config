#include "strings.h"

#include <qcommandlineparser.h>
#include <qcontainerfwd.h>
#include <qcoreapplication.h>

void usage(const QString &arg = "") {
  QCommandLineParser usageParser;
  usageParser.addHelpOption();
}

int main(int   argc,
         char *argv[]) {
  if (argc < 2) {
    usage();
    return 0;
  }
  QStringList splitArgs;

  for (auto i = 0; i < argc; ++i) {
    splitArgs.append(argv[i]);
  }

  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addOption({
      {"h", "help"},
      "Show application help.", "cmd"
  });

  parser.parse(splitArgs);
}
