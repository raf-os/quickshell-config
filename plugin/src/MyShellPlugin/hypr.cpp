#include "hypr.h"

#include <optional>
#include <utility>

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
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtenvironmentvariables.h>
#include <qtimer.h>

#include "hyprevents.h"
#include "hyprland.h"
#include "kbd.h"
#include "paths.h"

namespace myqmlplugin {
HyprKeyboardLayout::HyprKeyboardLayout(const QString &layout,
                                       const QString &variant,
                                       QObject       *parent)
    : QObject(parent),
      m_layout(layout),
      m_variant(variant) {}

HyprKeyboardLayout::HyprKeyboardLayout(const QString &layout,
                                       const QString &variant,
                                       const QString &description,
                                       QObject       *parent)
    : QObject(parent),
      m_layout(layout),
      m_variant(variant),
      m_description(description) {}

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
  if (m_layout == "") return false;
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

bool HyprInputConfig::setLayouts(const QList<std::pair<QString,
                                                       QString>> &layouts) {
  if (m_kbLayoutHandler == nullptr) {
    qWarning() << "myqmlplugin::HyprInputConfig::setLayouts: Must assign a "
                  "keyboard layout handler to set layouts.";
    return false;
  }

  bool isDifferent = false;
  if (layouts.size() != m_layouts.size()) {
    isDifferent = true;
  } else {
    for (const auto layout : m_layouts) {
      if (!layouts.contains(std::pair<QString, QString>(layout->layout(),
                                                        layout->variant()))) {
        isDifferent = true;
        break;
      }
    }
  }
  if (!isDifferent) return false;

  for (const auto item : m_layouts) {
    item->deleteLater();
  }

  m_layouts.clear();

  for (auto &layout : layouts) {
    auto k = m_kbLayoutHandler->findLayoutMetadata(std::move(layout.first),
                                                   std::move(layout.second));
    if (!k.has_value()) continue;
    auto kb = k.value();
    auto ly =
        new HyprKeyboardLayout(kb.layout, kb.variant, kb.description, this);
    m_layouts.append(ly);
  }

  emit layoutsChanged();
  return true;
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

std::optional<QByteArray> HyprInputConfig::compileCommandFileString() {
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

  QByteArray writeBuffer;

  writeBuffer.append("-- WARNING!\n"
                     "-- This file is automatically generated by the "
                     "shell, avoid making changes\n\n"
                     "hl.config({\n"
                     "\tinput = {\n"
                     "\t\tkb_layout = \"");
  writeBuffer.append(layouts.join(",").toUtf8());
  writeBuffer.append("\",\n\t\tkb_variant = \"");
  if (hasVariants) {
    writeBuffer.append(variants.join(",").toUtf8());
  }
  writeBuffer.append("\",\n\t\tkb_model = \"");
  if (!m_kbModel.isEmpty()) writeBuffer.append(m_kbModel.toUtf8());
  writeBuffer.append("\",\n\t\tkb_options = \"");
  if (!m_kbOptions.isEmpty()) writeBuffer.append(m_kbOptions.toUtf8());
  writeBuffer.append("\",\n\t\tkb_rules = \"");
  if (!m_kbRules.isEmpty()) writeBuffer.append(m_kbRules.toUtf8());
  writeBuffer.append("\"\n\t},\n})");

  return writeBuffer;
}

HyprExtras::HyprExtras(QObject *parent) : QObject(parent) {
  m_lookupCooldownTimer = new QTimer(this);

  m_inputConfig = new HyprInputConfig(this);

  m_hyprEvents = ns::hyprland::Hyprland::instance()->eventHandler();
  // m_hyprEvents->connectSocket();

  m_configPath = utils::Paths::instance()->config();
  m_cachePath  = utils::Paths::instance()->cache();

  QObject::connect(m_hyprEvents,
                   &ns::hyprland::HyprEvents::configReloaded,
                   this,
                   &HyprExtras::queryHyprInputConfigs);
  QObject::connect(m_hyprEvents,
                   &ns::hyprland::HyprEvents::keyboardLayoutChanged,
                   this,
                   &HyprExtras::queryCurrentDevices);
}

void HyprExtras::hyprctl(const QStringList &commands) {
  if (m_hyprctlProcess) {
    if (m_hyprctlProcess->state() != QProcess::NotRunning) {
      qDebug()
          << "myqmlplugin::HyprExtras::hyprctl: Process is busy, aborting.";
      return;
    }
  } else {
    m_hyprctlProcess = new QProcess(this);
  }

  m_hyprctlProcess->setProgram("hyprctl");
  m_hyprctlProcess->setArguments(commands);
  m_hyprctlProcess->start();

  QObject::connect(m_hyprctlProcess, &QProcess::finished, this, [this]() {
    m_hyprctlProcess->deleteLater();
    m_hyprctlProcess = nullptr;
  });
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

myqmlplugin::KeyboardLayoutHandler *HyprExtras::keyboardLayoutHandler() const {
  return m_kbLayoutHandler;
}

myqmlplugin::HyprInputConfig *HyprExtras::inputConfig() const {
  return m_inputConfig;
}

ns::hyprland::HyprEvents *HyprExtras::eventListener() const {
  return m_hyprEvents;
}

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
  if (m_ipProcessBuffer.size() == 0) return;

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
            auto        isMain = kbInfo["main"].toBool(false);

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

/**
 * Queries the current input configs from hyprland.
 *
 * The actual query is debounced as to not overload the compositor with several
 * calls and hang up the entire system.
 */
void HyprExtras::queryHyprInputConfigs() {
  if (m_hyprInputQueryProcess != nullptr) {
    if (m_hyprInputQueryProcess->state() != QProcess::NotRunning) {
      return;
    }
  } else {
    m_hyprInputQueryProcess = new QProcess(this);
  }

  if (!m_hyprInputQueryDebouncer) {
    m_hyprInputQueryDebouncer = new QTimer();
    m_hyprInputQueryDebouncer->setSingleShot(true);
    m_hyprInputQueryDebouncer->setInterval(100);
  }

  m_hyprInputQueryProcess->setProgram("hyprctl");
  m_hyprInputQueryProcess->setArguments(
      {"-j",
       "--batch",
       "getoption input.kb_layout;getoption input.kb_variant;getoption "
       "input.kb_model;getoption input.kb_options;getoption input.kb_rules"});

  QObject::connect(
      m_hyprInputQueryProcess, &QProcess::finished, this, [this]() {
        auto buf = m_hyprInputQueryProcess->readAllStandardOutput();
        parseHyprInputConfigs(buf);
      });

  QObject::connect(m_hyprInputQueryDebouncer, &QTimer::timeout, [this]() {
    if (m_hyprInputQueryProcess) {
      if (m_hyprInputQueryProcess->state() == QProcess::NotRunning) {
        m_hyprInputQueryProcess->start();
      }
    }
  });

  m_hyprInputQueryDebouncer->start();
}

void HyprExtras::parseHyprInputConfigs(QByteArray &buf) {
  if (buf.size() == 0) return;

  QStringList layoutBuf;
  QStringList variantsBuf;
  QString     modelBuf;
  QString     optBuf;
  QString     rulesBuf;

  auto cmdstr = QString::fromUtf8(buf).trimmed().split("\n\n\n");
  for (const auto line : cmdstr) {
    auto jDoc = QJsonDocument::fromJson(line.toLocal8Bit());
    if (jDoc.isObject()) {
      QJsonObject obj     = jDoc.object();
      auto        optName = obj["option"].toString("");
      if (optName == "input.kb_layout") {
        layoutBuf = obj["str"].toString("us").trimmed().split(",");
      } else if (optName == "input.kb_variants") {
        variantsBuf = obj["str"].toString("").trimmed().split(",");
      } else if (optName == "input.kb_model") {
        modelBuf = obj["str"].toString("").trimmed();
      } else if (optName == "input.kb_options") {
        optBuf = obj["str"].toString("").trimmed();
      } else if (optName == "input.kb_rules") {
        rulesBuf = obj["str"].toString("").trimmed();
      }
    }
  }

  if (layoutBuf.size() > 1) {
    if ((variantsBuf.length() > 1 ||
         (variantsBuf.length() == 1 && variantsBuf.at(0) != "")) &&
        layoutBuf.length() != variantsBuf.length()) {
      qWarning() << "myqmlplugin::HyprExtras::parseHyprInputConfigs: Invalid "
                    "input config detected.";
      return;
    }
  }

  m_inputConfig->setLayouts(layoutBuf, variantsBuf);
  m_inputConfig->setKbModel(modelBuf);
  m_inputConfig->setKbOptions(optBuf);
  m_inputConfig->setKbRules(rulesBuf);

  // saveDataToCache();
}

void HyprExtras::initConfigParse() { queryHyprInputConfigs(); }

/**
 * DEPRECATED
 */
void HyprExtras::hyprlangParse() {
  qWarning() << "myqmlplugin::HyprExtras::hyprlangParse: This function has "
                "been deprecated. Use queryHyprInputConfigs() instead.";
}

void HyprExtras::writeInputConfigToFile() {
  auto b = m_inputConfig->compileCommandFileString();
  if (!b.has_value()) {
    qWarning() << "myqmlplugin::HyprExtras::writeInputConfigToFile: Error "
                  "saving config - write buffer is empty.";
    return;
  }

  auto buffer = b.value();
  saveInputConfig(buffer);
}

void HyprExtras::saveInputConfig(const QByteArray &writeBuffer) {
  if (m_configPath == "") return;

  if (writeBuffer == nullptr) {
    return;
  }

  QFileInfo fileInfo(m_configPath + "/myshell/input.lua");

  QDir dir = fileInfo.absoluteDir();
  if (!dir.exists()) {
    dir.mkpath(".");
  }

  QFile file(fileInfo.canonicalFilePath());

  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << writeBuffer;

    file.close();

    emit inputConfigSaved();
  } else {
    qWarning() << "myqmlplugin::HyprExtras::saveInputConfig: Failed opening "
                  "input file.";
  }
}

void HyprExtras::saveDataToCache() {
  if (m_cachePath == "") {
    return;
  }

  QJsonObject kbdCfg;
  QJsonArray  hyprKeyboards;

  for (auto const &lay : m_inputConfig->layoutList()) {
    QJsonObject kbObj;
    kbObj["layout"]      = lay->layout();
    kbObj["variant"]     = lay->variant();
    kbObj["description"] = lay->description();
    hyprKeyboards.append(kbObj);
  }

  kbdCfg["hyprKeyboards"]       = hyprKeyboards;
  kbdCfg["hyprKeyboardModel"]   = m_inputConfig->kbModel();
  kbdCfg["hyprKeyboardRules"]   = m_inputConfig->kbRules();
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

std::optional<KeyboardLayoutHandler::SLayoutMetadata>
HyprExtras::getLayout(const QString &layout,
                      const QString &variant) {
  if (m_kbLayoutHandler == nullptr) return std::nullopt;
  return m_kbLayoutHandler->findLayoutMetadata(layout, variant);
}

void HyprExtras::changeSettings(
    std::optional<QList<std::pair<QString,
                                  QString>>> newLayouts,
    std::optional<int>                       newIndex) {
  if (newLayouts.has_value()) {
    auto list = newLayouts.value();
    if (list.isEmpty() || list.size() <= 1) {
      qWarning() << "myqmlplugin::HyprExtras::changeSettings: newLayouts must "
                    "have at least one item!";
      return;
    }
    auto succ = m_inputConfig->setLayouts(newLayouts.value());
    if (succ) {
      writeInputConfigToFile();
    }
  }

  if (newIndex.has_value()) {
    auto idx = newIndex.value();

    if (idx < 0) idx = 0;

    if (const auto maxSize = m_inputConfig->layoutList().size() - 1;
        idx > maxSize)
      idx = maxSize;

    if (newLayouts.has_value() || m_kbLayoutIndex != idx)
      hyprctl({"switchxkblayout", "current", QString::number(idx)});
  }
}
} // namespace myqmlplugin
