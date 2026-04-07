#include "hypr.h"

#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfiledevice.h>
#include <qfileinfo.h>
#include <qlogging.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qstringview.h>

namespace myqmlplugin {
HyprKeyboardLayout::HyprKeyboardLayout(QObject *parent) : QObject(parent) {}

QString HyprKeyboardLayout::layout() const { return m_layout; }

QString HyprKeyboardLayout::variant() const { return m_variant; }

bool HyprKeyboardLayout::isValid() {
  if (m_layout == "")
    return false;
  return true;
}

HyprInputConfig::HyprInputConfig(QObject *parent) : QObject(parent) {}

QString HyprInputConfig::kbModel() const { return m_kbModel; }

QString HyprInputConfig::kbOptions() const { return m_kbOptions; }

QString HyprInputConfig::kbRules() const { return m_kbRules; }

QQmlListProperty<HyprKeyboardLayout> HyprInputConfig::layouts() {
  return QQmlListProperty<HyprKeyboardLayout>(this, &m_layouts);
}

HyprExtras::HyprExtras(QObject *parent) : QObject(parent) {}

QString HyprExtras::configpath() const { return m_configPath; }

void HyprExtras::setConfigPath(const QString &path) {
  if (m_configPath != path) {
    m_configPath = path;
    emit configPathChanged();
  }
}

void HyprExtras::parseInputConfig() {
  QDir cfgDir(m_configPath);
  if (!cfgDir.exists()) {
    qWarning()
        << "myshellplugin::HyprExtras: Hyprland config path is not set up "
           "correctly. Some Hyprland specific features will not work "
           "correctly.";
    return;
  }
  const QString inputConfPath = "/myshell/input.conf";
  QFile file(m_configPath + inputConfPath);

  if (!file.exists()) {
    qWarning() << "myshellplugin::HyprExtras: Missing input config file. "
                  "Skipping parsing.";
    return;
  }

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "myshellplugin::HyprExtras: Unknown error reading input "
                  "config file.";
    return;
  }

  bool inputSectionFlag = false;

  bool isError = false;

  QMap<QString, QString> settingsBuffer;

  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine().trimmed();
    QString lineBuffer;
    QString cmdBuffer;
    QString valBuffer;

    bool isSuffix = false;

    for (auto it = line.cbegin(); it != line.cend(); ++it) {
      if (QString::compare(it, "#") == 0)
        break;
      if (QString::compare(it, "{") == 0) {
        if (lineBuffer.startsWith("input")) {
          inputSectionFlag = true;
          lineBuffer = "";
        }
      } else if (QString::compare(it, "}") == 0) {
        if (inputSectionFlag == true) {
          inputSectionFlag = false;
          lineBuffer = "";
          break;
        }
      } else if (QString::compare(it, "=") == 0 && inputSectionFlag) {
        if (!isSuffix) {
          cmdBuffer = lineBuffer.trimmed();
          lineBuffer = "";
          isSuffix = true;
        } else {
          isError = true;
          break;
        }
      } else {
        lineBuffer += *it;
      }
    }

    if (isSuffix) {
      valBuffer = lineBuffer.trimmed();
    }

    if (isError) {
      break;
    }
  }

  file.close();
}
} // namespace myqmlplugin
