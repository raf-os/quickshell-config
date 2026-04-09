#include "hypr.h"
#include "kbd.h"

#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfiledevice.h>
#include <qfileinfo.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtimer.h>

namespace myqmlplugin {
HyprKeyboardLayout::HyprKeyboardLayout(const QString &layout,
                                       const QString &variant, QObject *parent)
    : QObject(parent), m_layout(layout), m_variant(variant) {}

QString HyprKeyboardLayout::layout() const { return m_layout; }

QString HyprKeyboardLayout::variant() const { return m_variant; }

QString HyprKeyboardLayout::description() const { return m_description; }

void HyprKeyboardLayout::setDescription(const QString &desc) {
  if (m_description != desc) {
    m_description = desc;
    emit descriptionChanged();
  }
}

bool HyprKeyboardLayout::isValid() {
  if (m_layout == "")
    return false;
  return true;
}

HyprInputConfig::HyprInputConfig(QObject *parent) : QObject(parent) {}

void HyprInputConfig::attachKeyboardHandler(KeyboardLayoutHandler *obj) {
  m_kbLayoutHandler = obj;
}

QString HyprInputConfig::kbModel() const { return m_kbModel; }

void HyprInputConfig::setKbModel(const QString &model) {
  if (model != m_kbModel) {
    m_kbModel = model;
    emit kbModelChanged();
  }
}

QString HyprInputConfig::kbOptions() const { return m_kbOptions; }

void HyprInputConfig::setKbOptions(const QString &opts) {
  if (opts != m_kbOptions) {
    m_kbOptions = opts;
    emit kbOptionsChanged();
  }
}

QString HyprInputConfig::kbRules() const { return m_kbRules; }

void HyprInputConfig::setKbRules(const QString &rules) {
  if (rules != m_kbRules) {
    m_kbRules = rules;
    emit kbRulesChanged();
  }
}

QQmlListProperty<HyprKeyboardLayout> HyprInputConfig::layouts() {
  return QQmlListProperty<HyprKeyboardLayout>(this, &m_layouts);
}

void HyprInputConfig::setLayouts(const QStringList &layouts,
                                 const QStringList &variants) {
  if (variants.size() > 1 && variants.size() != layouts.size()) {
    // If this function is being called this was likely already checked, but
    // just to be sure...
    qWarning() << "myqmlplugin::HyprInputConfig::setLayouts: layout and "
                  "variants differ size - configuration is invalid.";
    return;
  }

  bool isDifferentFlag = false;

  // TODO: There's probably a better comparison algorithm

  if (layouts.size() != m_layouts.size()) {
    isDifferentFlag = true;
  } else {
    for (int i = 0; i < layouts.count(); ++i) {
      bool contains = false;
      for (int j = 0; j < m_layouts.count(); ++j) {
        if (m_layouts.at(j)->layout() == layouts.at(i)) {
          if (variants.size() > 1) {
            if (m_layouts.at(j)->variant() == variants.at(i)) {
              contains = true;
              break;
            }
          } else {
            contains = true;
            break;
          }
        }
      }

      if (contains == false) {
        isDifferentFlag = true;
        break;
      }
    }
  }

  if (isDifferentFlag) {
    for (auto item : m_layouts) {
      item->deleteLater();
    }

    m_layouts.clear();

    for (int i = 0; i < layouts.count(); ++i) {
      QString vBuf = "";
      if (variants.size() > 1) {
        vBuf = variants.at(i);
      }

      auto cfg = new HyprKeyboardLayout(layouts.at(i), vBuf, this);
      m_layouts.append(cfg);
    }

    emit layoutsChanged();
  }
}

HyprExtras::HyprExtras(QObject *parent) : QObject(parent) {
  m_lookupCooldownTimer = new QTimer(this);
  m_lookupCooldownTimer->setSingleShot(true);
  m_lookupCooldownTimer->setInterval(250);

  m_inputConfig = new HyprInputConfig(this);
}

int HyprExtras::kbdLayoutIndex() const { return m_kbLayoutIndex; }

QString HyprExtras::configPath() const { return m_configPath; }

void HyprExtras::setConfigPath(const QString &path) {
  if (m_configPath != path) {
    m_configPath = path;
    emit configPathChanged();
  }
}

QString HyprExtras::shellConfigPath() const { return m_shellConfigPath; }

void HyprExtras::setShellConfigPath(const QString &path) {
  if (m_shellConfigPath != path) {
    if (!QDir(m_shellConfigPath).exists()) {
      qWarning() << "myshellplugin::HyprExtras::setShellConfigPath: Invalid "
                    "shell config path provided.";
      m_shellConfigPath = "";
      return;
    }
    m_shellConfigPath = path;
    emit shellConfigPathChanged();
  }
}

void HyprExtras::updateCurrentKeyboardConfig() { return queryCurrentDevices(); }

KeyboardLayoutHandler *HyprExtras::keyboardLayoutHandler() const {
  return m_kbLayoutHandler;
}

HyprInputConfig *HyprExtras::inputConfig() const { return m_inputConfig; }

void HyprExtras::setKeyboardLayoutHandler(KeyboardLayoutHandler *kbd) {
  if (m_kbLayoutHandler == nullptr || kbd != m_kbLayoutHandler) {
    m_kbLayoutHandler = kbd;

    m_inputConfig->attachKeyboardHandler(kbd);
    emit keyboardLayoutHandlerChanged();
  }
}

void HyprExtras::queryCurrentDevices() {
  if (m_lookupCooldownTimer != nullptr && m_lookupCooldownTimer->isActive()) {
    return;
  }

  if (m_inputQueryProcess != nullptr) {
    if (m_inputQueryProcess->state() != QProcess::NotRunning) {
      return;
    }
  } else {
    m_inputQueryProcess = new QProcess(this);
  }

  m_ipProcessBuffer.clear();

  m_inputQueryProcess->setProgram("hyprctl");
  m_inputQueryProcess->setArguments({"devices", "-j"});

  QObject::connect(m_inputQueryProcess, &QProcess::finished, this, [this]() {
    auto buf = m_inputQueryProcess->readAllStandardOutput();
    m_ipProcessBuffer.append(buf);
    m_lookupCooldownTimer->start();

    this->parseProcessData();
  });

  m_inputQueryProcess->start();
}

void HyprExtras::parseProcessData() {
  if (m_ipProcessBuffer.size() == 0)
    return;

  QJsonParseError parseError;
  QJsonDocument jDoc = QJsonDocument::fromJson(m_ipProcessBuffer, &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    qWarning() << "myqmlplugin::HyprExtras::parseProcessData: Error parsing "
                  "process json data:"
               << parseError.errorString();
    return;
  }

  m_ipProcessBuffer.clear();

  if (jDoc.isObject()) {
    QJsonObject obj = jDoc.object();

    auto it = obj.find("keyboards");
    if (it != obj.end()) {
      QJsonValue val = it.value();

      if (val.isArray()) {
        QJsonArray keyboards = val.toArray();

        for (const QJsonValue &value : keyboards) {
          if (value.isObject()) {
            QJsonObject kbInfo = value.toObject();
            auto isMain = kbInfo["main"].toBool(false);

            if (isMain) {
              auto layoutIdx = kbInfo["active_layout_index"].toInt(0);

              if (layoutIdx != m_kbLayoutIndex) {
                m_kbLayoutIndex = layoutIdx;
                emit kbdLayoutIndexChanged();
              }

              break;
            }
          }
        }
      }
    }
  } else {
    qWarning() << "myqmlplugin::HyprExtras::parseProcessData: Invalid json "
                  "data received.";
    return;
  }
}

void HyprExtras::initConfigParse() { parseInputConfig(); }

void HyprExtras::parseInputConfig() {
  QDir cfgDir(m_configPath);
  if (!QDir(m_configPath).exists()) {
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
  bool isSuffixFlag = false;

  bool isError = false;

  QMap<QString, QString> settingsBuffer;

  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine().trimmed();
    QString lineBuffer;
    QString cmdBuffer;
    QString valBuffer;

    isSuffixFlag = false;

    for (auto it = line.cbegin(); it != line.cend(); ++it) {
      if (QString::compare(it, "#") == 0)
        break;

      lineBuffer += *it;

      if (QString::compare(it, "{") == 0) {
        if (inputSectionFlag) {
          isError = true;
          break;
        }
        if (lineBuffer == "input {") {
          inputSectionFlag = true;
          lineBuffer = "";
        }
        break;
      } else if (QString::compare(it, "}") == 0) {
        if (inputSectionFlag == true) {
          inputSectionFlag = false;
          lineBuffer = "";
        }
      }
    }

    if (isError) {
      break;
    }

    if (inputSectionFlag) {
      if (lineBuffer.contains("=")) {

        auto splitStr = lineBuffer.split("=");

        cmdBuffer = splitStr[0].trimmed();
        if (cmdBuffer.length() == 0)
          break;
        valBuffer = splitStr[1].trimmed();
        settingsBuffer.insert(cmdBuffer, valBuffer);

        // qDebug() << cmdBuffer << ":" << valBuffer << "\n";
      }
    }
  }

  if (isError) {
    qDebug() << "error occurred";
  }

  file.close();

  QStringList layouts;
  QStringList variants;

  if (settingsBuffer.contains("kb_layout")) {
    layouts = settingsBuffer.value("kb_layout", "").split(",");
    variants = settingsBuffer.value("kb_variant", "").split(",");

    auto variantsSize = variants.size();

    if (variants.size() > 1 && layouts.size() != variants.size()) {
      // invalid config
      return;
    }
  }

  auto kbModel = settingsBuffer.value("kb_model", "");
  auto kbOptions = settingsBuffer.value("kb_options", "");
  auto kbRules = settingsBuffer.value("kb_rules", "");

  m_inputConfig->setLayouts(layouts, variants);
  m_inputConfig->setKbModel(kbModel);
  m_inputConfig->setKbOptions(kbOptions);
  m_inputConfig->setKbRules(kbRules);
}
} // namespace myqmlplugin
