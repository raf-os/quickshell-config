#include "hypr.h"
#include "kbd.h"

#include <hyprlang.hpp>
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
#include <qlist.h>
#include <qlogging.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtimer.h>
#include <string>

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
      if (m_kbLayoutHandler != nullptr) {
        auto k = m_kbLayoutHandler->findLayoutByName(layouts.at(i));
        if (k != nullptr) {
          QString descriptionBuffer = k->description();
          if (vBuf != "") {
            if (auto kv = k->getVariantByName(vBuf); kv != nullptr) {
              auto kvDesc = kv->description();
              if (kvDesc != "") {
                // Only override parent's description if one exists
                descriptionBuffer = kvDesc;
              }
            }
          }
          cfg->setDescription(descriptionBuffer);
        }
      }
      m_layouts.append(cfg);
    }

    emit layoutsChanged();
  }
}

QList<HyprKeyboardLayout *> HyprInputConfig::layoutList() const {
  return m_layouts;
}

QByteArray *HyprInputConfig::tryFetchWriteBuffer() {
  if (!m_bufferReadyFlag) {
    return nullptr;
  }
  return &m_confWriteBuffer;
}

bool HyprInputConfig::compileCommandFileString() {
  if (m_bufferReadyFlag == false)
    return false;

  m_bufferReadyFlag = false;

  QStringList layouts;
  QStringList variants;

  for (auto l : m_layouts) {
    layouts.append(l->layout());
    variants.append(l->variant());
  }

  bool hasVariants;

  for (auto v : variants) {
    if (v != "") {
      hasVariants = true;
      break;
    }
  }

  if (!hasVariants) {
    variants.clear();
  }

  m_confWriteBuffer.clear();

  m_confWriteBuffer.append(
      "# This file is auto-generated, avoid making changes\n\ninput {");
  m_confWriteBuffer.append("\nkb_layout = ");
  m_confWriteBuffer.append(layouts.join(",").toUtf8());
  m_confWriteBuffer.append("\nkb_variant = ");
  if (hasVariants) {
    m_confWriteBuffer.append(variants.join(",").toUtf8());
  }
  m_confWriteBuffer.append("\nkb_model = ");
  m_confWriteBuffer.append(m_kbModel.toUtf8());
  m_confWriteBuffer.append("\nkb_options = ");
  m_confWriteBuffer.append(m_kbOptions.toUtf8());
  m_confWriteBuffer.append("\nkb_rules = ");
  m_confWriteBuffer.append(m_kbRules.toUtf8());
  m_confWriteBuffer.append("\n}");

  emit fileBufferReadyToWrite();

  m_bufferReadyFlag = true;
  return true;
}

HyprExtras::HyprExtras(QObject *parent) : QObject(parent) {
  m_lookupCooldownTimer = new QTimer(this);

  m_inputConfig = new HyprInputConfig(this);

  QObject::connect(m_inputConfig, &HyprInputConfig::fileBufferReadyToWrite,
                   this, [this]() { this->saveInputConfig(); });
}

HyprExtras::~HyprExtras() = default;

/**
 * Holds the pointer to the current config instance momentarily
 */
Hyprlang::CConfig *HyprExtras::s_hyprlangConfig = nullptr;

/**
 * Ideally this would've been an inline lambda function, but because hyprlang
 * does things in a C way, it requires a C-style function pointer - meaning you
 * can't have captures. So a static function it is.
 */
Hyprlang::CParseResult HyprExtras::hyprlangHandleSource(const char *COMMAND,
                                                        const char *VALUE) {
  // TODO: Maybe watch all these files for any changes to automatically reload
  // the configs
  QString path;
  auto valStr = QString::fromUtf8(VALUE);
  if (valStr.startsWith("~/")) {
    QString tempPath = QDir::homePath() + valStr.sliced(1);
    path = QFileInfo(tempPath).canonicalFilePath();
  } else {
    path = valStr;
  }
  return HyprExtras::s_hyprlangConfig->parseFile(path.toUtf8().constData());
}

int HyprExtras::kbdLayoutIndex() const { return m_kbLayoutIndex; }

bool HyprExtras::isSaving() const { return m_isSavingFlag; }

void HyprExtras::setIsSaving(bool val) {
  if (m_isSavingFlag != val) {
    m_isSavingFlag = val;
    emit isSavingChanged();
  }
}

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
    if (!QDir(path).exists()) {
      qWarning() << "myshellplugin::HyprExtras::setShellConfigPath: Invalid "
                    "shell config path provided.";
      m_shellConfigPath = "";
      return;
    }
    m_shellConfigPath = path;
    emit shellConfigPathChanged();
  }
}

QString HyprExtras::cachePath() const { return m_cachePath; }

void HyprExtras::setCachePath(const QString &path) {
  if (m_cachePath != path) {
    if (!QDir(path).exists()) {
      qWarning() << "myshellplugin::HyprExtras::setCachePath: Invalid cache "
                    "path provided.";
      m_cachePath = "";
      return;
    }
    m_cachePath = path;
    emit cachePathChanged();
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
    m_lookupCooldownTimer->setSingleShot(true);
    m_lookupCooldownTimer->setInterval(250);
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

void HyprExtras::initConfigParse() { hyprlangParse(); }

void HyprExtras::hyprlangParse() {
  if (m_configPath == "")
    return;
  QString cfgPath = m_configPath + "/hyprland.conf";
  Hyprlang::CConfig config(cfgPath.toUtf8(), {.allowMissingConfig = true});

  // I don't like this
  HyprExtras::s_hyprlangConfig = &config;

  // TODO: Define these keys as constant strings, one mistake already caused
  // quite the headache debugging this
  config.addConfigValue("input:kb_layout", (Hyprlang::STRING) "us");
  config.addConfigValue("input:kb_variant", (Hyprlang::STRING) "");
  config.addConfigValue("input:kb_model", (Hyprlang::STRING) "");
  config.addConfigValue("input:kb_options", (Hyprlang::STRING) "");
  config.addConfigValue("input:kb_rules", (Hyprlang::STRING) "");

  config.registerHandler(&hyprlangHandleSource, "source",
                         {.allowFlags = false});
  // May this shield us from C memory management sins
  HandlerGuard guard;

  config.commence();

  const auto PARSERRESULT = config.parse();

  auto kbLayouts = QString::fromStdString(
      *Hyprlang::CSimpleConfigValue<std::string>(&config, "input:kb_layout"));
  auto kbVariants = QString::fromStdString(
      *Hyprlang::CSimpleConfigValue<std::string>(&config, "input:kb_variant"));
  auto kbModel = QString::fromStdString(
      *Hyprlang::CSimpleConfigValue<std::string>(&config, "input:kb_model"));
  auto kbOptions = QString::fromStdString(
      *Hyprlang::CSimpleConfigValue<std::string>(&config, "input:kb_options"));
  auto kbRules = QString::fromStdString(
      *Hyprlang::CSimpleConfigValue<std::string>(&config, "input:kb_rules"));

  QStringList layouts;
  QStringList variants;

  if (kbLayouts.length() > 0) {
    layouts = kbLayouts.trimmed().split(",");
    variants = kbVariants.trimmed().split(",");

    if ((variants.length() > 1 ||
         (variants.length() == 1 && variants.at(0) != "")) &&
        layouts.length() != variants.length()) {
      qWarning() << "myshellplugin::HyprExtras::hyprlangParse: Invalid config "
                    "detected";
      // INVALID
      return;
    }
  }

  m_inputConfig->setLayouts(layouts, variants);
  m_inputConfig->setKbModel(kbModel);
  m_inputConfig->setKbOptions(kbOptions);
  m_inputConfig->setKbRules(kbRules);

  config.unregisterHandler("source");
  HyprExtras::s_hyprlangConfig = nullptr;

  saveDataToCache();
}

void HyprExtras::writeInputConfigToFile() {
  if (m_isSavingFlag)
    return;
  setIsSaving(m_inputConfig->compileCommandFileString());
}

void HyprExtras::saveInputConfig() {
  if (m_configPath == "")
    return;

  auto writeBuffer = m_inputConfig->tryFetchWriteBuffer();

  if (writeBuffer == nullptr) {
    return;
  }

  QFileInfo fileInfo(m_configPath + "/myshell/input.conf");

  QDir dir = fileInfo.absoluteDir();
  if (!dir.exists()) {
    dir.mkpath(".");
  }

  QFile file(fileInfo.canonicalFilePath());

  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << *writeBuffer;

    file.close();

    writeBuffer->clear();
    emit inputConfigSaved();
  } else {
    qWarning() << "myqmlplugin::HyprExtras::saveInputConfig: Failed opening "
                  "input file.";
  }

  setIsSaving(false);
}

void HyprExtras::saveDataToCache() {
  if (m_cachePath == "") {
    return;
  }

  QJsonObject kbdCfg;
  QJsonArray hyprKeyboards;

  for (auto const &lay : m_inputConfig->layoutList()) {
    QJsonObject kbObj;
    kbObj["layout"] = lay->layout();
    kbObj["variant"] = lay->variant();
    kbObj["description"] = lay->description();
    hyprKeyboards.append(kbObj);
  }

  kbdCfg["hyprKeyboards"] = hyprKeyboards;
  kbdCfg["hyprKeyboardModel"] = m_inputConfig->kbModel();
  kbdCfg["hyprKeyboardRules"] = m_inputConfig->kbRules();
  kbdCfg["hyprKeyboardOptions"] = m_inputConfig->kbOptions();

  QJsonDocument jDoc(kbdCfg);

  QFile file(m_cachePath + "/hyprKeyboards.json");

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "myqmlplugin::HyprExtras::saveDataToCache: Error opening "
                  "cache file for writing.";
    return;
  }

  QTextStream out(&file);

  out << jDoc.toJson(QJsonDocument::Compact);

  file.close();
}
} // namespace myqmlplugin
