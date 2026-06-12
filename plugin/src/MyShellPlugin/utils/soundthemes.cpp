#include "soundthemes.h"

#include <algorithm>
#include <optional>
#include <qbuffer.h>
#include <qdir.h>
#include <qhash.h>
#include <qlist.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qregularexpression.h>
#include <qstringview.h>
#include <utility>

using StringHash = QHash<QString, QString>;

namespace ns {
SoundThemes::SoundThemes(QObject *parent) : QObject(parent) { buildThemeDb(); }

QQmlListProperty<SoundThemeData> SoundThemes::themes() {
  return QQmlListProperty<SoundThemeData>(this, &m_themes);
}

void SoundThemes::buildThemeDb() {
  QList<SThemeData> themeBuffer;

  const QList<QString> searchPaths{"/usr/share/sounds",
                                   "/usr/local/share/sounds",
                                   QDir::homePath() + "/.local/share/sounds"};
  for (const QString &path : searchPaths) {
    QDir dir(path);
    if (!dir.exists())
      continue;

    for (const auto entry :
         dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable)) {
      QFile indexFile(dir.canonicalPath() + "/" + entry + "/index.theme");
      if (!indexFile.exists())
        continue;

      auto res = parseThemeFile(indexFile);
      if (res.has_value()) {
        auto result = res.value();
        if (!result.contains("Sound Theme"))
          continue;
        auto st = result.value("Sound Theme");
        if (!st.contains("Name"))
          continue;

        auto n = st.value("Name");
        auto fn = indexFile.fileName();
        themeBuffer.append({n, entry, fn});
      }
    }
  }

  bool isChanged = false;
  for (const auto &theme : themeBuffer) {
    bool hasTheme = false;
    for (const auto &mtheme : m_themes) {
      if (mtheme->themeName() == theme.name) {
        hasTheme = true;
        break;
      }
    }
    if (hasTheme == false) {
      isChanged = true;
      break;
    }
  }

  if (!isChanged)
    return;

  for (auto &theme : m_themes) {
    theme->deleteLater();
  }

  m_themes.clear();

  for (auto &buf : themeBuffer) {
    auto t = new SoundThemeData(std::move(buf), this);
    m_themes.append(t);
  }

  std::sort(m_themes.begin(), m_themes.end(),
            [this](SoundThemeData *a, SoundThemeData *b) {
              return a->themeName().localeAwareCompare(b->themeName()) < 0;
            });

  emit themesChanged();
}

std::optional<QHash<QString, StringHash>>
SoundThemes::parseThemeFile(QFile &file) {
  if (!file.exists())
    return std::nullopt;

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "ns::SoundThemes::parseThemeFile: Error opening theme file "
                  "for reading.";
    return std::nullopt;
  }

  QHash<QString, StringHash> table;
  QString currentSection = "__GLOBALS__";

  QRegularExpression headerRe("\\[(\\w[\\w\\s\\.\\\"\\']+)\\]");
  QRegularExpression assignRe("^\\s*(\\w+)\\s*=\\s*([^#;]+?)(?:\\s*[#;].*)?$");
  headerRe.optimize();
  assignRe.optimize();

  while (!file.atEnd()) {
    auto line = file.readLine().trimmed();

    if (line.startsWith(";") || line.startsWith("#"))
      continue;

    auto md = headerRe.match(line);

    if (auto match = headerRe.match(line); match.hasMatch()) {
      currentSection = match.captured(1);
      continue;
    }

    if (auto match = assignRe.match(line); match.hasMatch()) {
      if (table.contains(currentSection)) {
        auto val = table.value(currentSection);
        val.insert(match.captured(1), match.captured(2));
      } else {
        StringHash assignment = {{match.captured(1), match.captured(2)}};
        table.insert(currentSection, assignment);
      }
      continue;
    }
  }

  file.close();

  return table;
}
} // namespace ns
