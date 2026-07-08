#include "entryutils.h"
#include "localeutil.h"

#include <qloggingcategory.h>
#include <ranges>

namespace ns::desktop::entries {
Q_DECLARE_LOGGING_CATEGORY(logNSDesktopEntries)

const QStringList &EntryUtils::desktopPaths() {
  static const auto paths = []() {
    QStringList dataPaths;

    auto dataHome = qEnvironmentVariable("XDG_DATA_HOME");
    if (dataHome.isEmpty() && qEnvironmentVariableIsSet("HOME"))
      dataHome = qEnvironmentVariable("HOME") + "./local/share";
    if (!dataHome.isEmpty())
      dataPaths.append(dataHome + "/applications");

    auto dataDirs = qEnvironmentVariable("XDG_DATA_DIRS");
    if (dataDirs.isEmpty())
      dataDirs = "/usr/local/share:/usr/share";

    for (const auto &dir : dataDirs.split(':', Qt::SkipEmptyParts)) {
      dataPaths.append(dir + "/applications");
    }

    return dataPaths;
  }();

  return paths;
}

std::optional<EntryData> EntryUtils::parseText(const QString &id,
                                               const QString &filePath) {
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    qCDebug(logNSDesktopEntries)
        << "Failed opening file" << filePath << "for reading, skipping it...";
    return std::nullopt;
  }

  EntryData data;
  data.id                  = id;
  const auto &systemLocale = LocaleUtils::systemLocale();

  QString                                     groupName;
  QHash<QString, QPair<LocaleUtils, QString>> entries;

  QStringList                     actionOrder;
  QHash<QString, EntryActionData> pendingActions;

  auto processCategory =
      [&data, &groupName, &entries, &actionOrder, &pendingActions]() {
        if (groupName == "Desktop Entry") {
          if (entries.value("Type").second != "Application")
            return;

          for (const auto &[key, pair] : entries.asKeyValueRange()) {
            auto &[_, value] = pair;
            data.entries.insert(key, value);

            if (key == "Name")
              data.name = value;
            else if (key == "GenericName")
              data.genericName = value;
            else if (key == "StartupWMClass")
              data.startupClass = value;
            else if (key == "NoDisplay")
              data.noDisplay = value == "true";
            else if (key == "Comment")
              data.comment = value;
            else if (key == "Icon")
              data.icon = value;
            else if (key == "Exec") {
              data.execStr = value;
              data.command = EntryUtils::parseExecString(value);
            } else if (key == "Path")
              data.workingDirectory = value;
            else if (key == "Terminal")
              data.terminal = value == "true";
            else if (key == "Categories")
              data.categories = value.split(u';', Qt::SkipEmptyParts);
            else if (key == "Keywords")
              data.keywords = value.split(u';', Qt::SkipEmptyParts);
            else if (key == "Actions")
              actionOrder = value.split(u';', Qt::SkipEmptyParts);
          }
        } else if (groupName.startsWith("Desktop Action ")) {
          auto            actionName = groupName.sliced(15);
          EntryActionData action;
          action.id = actionName;

          for (const auto &[key, pair] : entries.asKeyValueRange()) {
            const auto &[_, value] = pair;
            action.entries.insert(key, value);

            if (key == "Name")
              action.name = value;
            else if (key == "Icon")
              action.icon = value;
            else if (key == "Exec") {
              action.execString = value;
            }
          }

          pendingActions.insert(actionName, action);
        }

        entries.clear();
      };

  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine().trimmed();

    if (line.isEmpty())
      continue;

    if (line.startsWith(u'#'))
      continue;

    if (line.startsWith(u'[') && line.endsWith(u']')) {
      processCategory();
      groupName = line.sliced(1, line.length() - 2);
      continue;
    }

    auto splitIdx = line.indexOf(u'=');
    if (splitIdx == -1) {
      continue;
    }

    auto        key   = line.sliced(0, splitIdx);
    const auto &value = line.sliced(splitIdx + 1);

    auto        localeIdx = key.indexOf('[');
    LocaleUtils locale;
    if (localeIdx != -1 && localeIdx != key.length() - 1) {
      locale =
          LocaleUtils(key.sliced(localeIdx + 1, key.length() - localeIdx - 2));
      key = key.sliced(0, localeIdx);
    }

    if (entries.contains(key)) {
      const auto &old = entries.value(key);

      auto oldScore = systemLocale.matchScore(old.first);
      auto newScore = systemLocale.matchScore(locale);

      if (newScore > oldScore || (oldScore == 0 && !locale.isValid())) {
        entries.insert(key, qMakePair(locale, value));
      }
    } else {
      entries.insert(key, qMakePair(locale, value));
    }
  }

  processCategory();

  for (const auto &actionId : actionOrder) {
    if (pendingActions.contains(actionId)) {
      data.actions.append(pendingActions.value(actionId));
    }
  }

  return data;
}

QStringList EntryUtils::parseExecString(const QString &execString) {
  QStringList arguments;
  QString     currentArgument;

  if (execString.isEmpty())
    return arguments;

  auto escape        = 0;
  auto parsingString = false;
  auto percent       = false;

  for (auto chr : execString) {
    if (escape == 0 && chr == u'\\') {
      escape = 1;
      continue;
    }

    if (parsingString) {
      if (chr == '\\') {
        escape++;
        if (escape == 4) {
          currentArgument += '\\';
          escape = 0;
        }
      } else if (escape == 2) {
        currentArgument += chr;
        escape = 0;
      } else if (escape != 0) {
        switch (chr.unicode()) {
        case 's':
          currentArgument += u' ';
          break;
        case 'n':
          currentArgument += u'\n';
          break;
        case 't':
          currentArgument += u't';
          break;
        case 'r':
          currentArgument += u'r';
          break;
        case '\\':
          currentArgument += u'\\';
          break;
        default:
          currentArgument += chr;
          break;
        }
        escape = 0;
      } else if (chr == u'"' || chr == u'\'') {
        parsingString = false;
      } else {
        currentArgument += chr;
      }
    } else if (escape != 0) {
      currentArgument += chr;
      escape = 0;
    } else if (percent) {
      if (chr == '%') {
        currentArgument += '%';
      }
      percent = false;
    } else if (chr == '%') {
      percent = true;
    } else if (chr == u'"' || chr == u'\'') {
      parsingString = true;
    } else if (chr == u' ') {
      if (!currentArgument.isEmpty()) {
        arguments.append(currentArgument);
        currentArgument.clear();
      }
    } else {
      currentArgument += chr;
    }
  }

  if (!currentArgument.isEmpty()) {
    arguments.append(currentArgument);
    currentArgument.clear();
  }

  return arguments;
}
} // namespace ns::desktop::entries
