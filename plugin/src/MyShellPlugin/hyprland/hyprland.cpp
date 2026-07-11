#include "hyprland.h"

#include <qcontainerfwd.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qprocess.h>
#include <qscopeguard.h>

#include "hyprevents.h"
#include "hyprinputconfig.h"

namespace ns::hyprland {
Q_LOGGING_CATEGORY(logNSHyprland,
                   "nightshell.hyprland")

Hyprland::Hyprland(QObject *parent)
    : QObject(parent),
      m_eventHandler(new HyprEvents(this)),
      m_inputConfig(new HyprInputConfig(this)),
      m_hyprInputQueryProcess(new QProcess(this)) {
  m_hyprInputQueryProcess->setProgram("hyprctl");
  m_hyprInputQueryProcess->setArguments(
      {"-j",
       "--batch",
       "getoption input.kb_layout;getoption input.kb_variant;getoption "
       "input.kb_model;getoption input.kb_options;getoption input.kb_rules"});
  QObject::connect(m_hyprInputQueryProcess,
                   &QProcess::finished,
                   this,
                   &Hyprland::onInputQueryReadyToRead);

  m_deviceQueryCooldown.setInterval(250);
  m_deviceQueryCooldown.setSingleShot(true);

  QObject::connect(m_eventHandler, &HyprEvents::configReloaded, this, [this] {
    if (this->m_deviceQueryQueued) {
      this->m_deviceQueryQueued = false;
      this->queryActiveDevices();
    }
  });
}

HyprEvents *Hyprland::eventHandler() { return m_eventHandler; }

int  Hyprland::keyboardLayoutIndex() const { return m_keyboardLayoutIndex; }
void Hyprland::setKeyboardLayoutIndex(const int &value) {
  if (m_keyboardLayoutIndex == value) return;

  m_keyboardLayoutIndex = value;
  emit keyboardLayoutIndexChanged();
}

void Hyprland::queryHyprInputConfigs() {
  if (m_hyprInputQueryProcess->state() != QProcess::NotRunning) {
    m_hyprInputQueryQueued = true;
    return;
  }

  m_hyprInputQueryProcess->start();
}

void Hyprland::onInputQueryReadyToRead() {
  const auto contents = m_hyprInputQueryProcess->readAllStandardOutput();

  QString     parsedModel, parsedOptions, parsedRules;
  QStringList parsedLayouts, parsedVariants;

  auto cmdList = QString::fromUtf8(contents).trimmed().split("\n\n\n");
  for (const auto &line : cmdList) {
    auto jDoc = QJsonDocument::fromJson(line.toUtf8());
    if (!jDoc.isObject()) continue;

    QJsonObject jObj    = jDoc.object();
    QString     optName = jObj.value("option").toString();
    auto        optVal  = jObj.value("str");
    if (optName.isEmpty()) continue;

    if (optName == "input.kb_layout") {
      parsedLayouts = optVal.toString("us").trimmed().split(",");
    } else if (optName == "input.kb_variants") {
      parsedVariants = optVal.toString("").trimmed().split(",");
    } else if (optName == "input.kb_model") {
      parsedModel = optVal.toString().trimmed();
    } else if (optName == "input.kb_options") {
      parsedOptions = optVal.toString().trimmed();
    } else if (optName == "input.kb_rules") {
      parsedRules = optVal.toString().trimmed();
    }
  }

  m_inputConfig->b_kbModel().setValue(parsedModel);
  m_inputConfig->b_kbOptions().setValue(parsedOptions);
  m_inputConfig->b_kbRules().setValue(parsedRules);
  m_inputConfig->setLayouts(parsedLayouts, parsedVariants);

  if (m_hyprInputQueryQueued) {
    m_hyprInputQueryQueued = false;
    this->queryHyprInputConfigs();
  }
}

void Hyprland::queryActiveDevices() {
  if (m_deviceQueryCooldown.isActive()) {
    m_deviceQueryQueued = true;
    return;
  }

  if (m_deviceQueryProcess != nullptr) {
    return;
  }

  m_deviceQueryProcess = new QProcess(this);
  m_deviceQueryProcess->setProgram("hyprctl");
  m_deviceQueryProcess->setArguments({
      {"devices", "-j"}
  });

  QObject::connect(m_deviceQueryProcess, &QProcess::finished, this, [this]() {
    auto guard = qScopeGuard([this] {
      if (m_deviceQueryProcess) {
        m_deviceQueryProcess->deleteLater();
        m_deviceQueryProcess = nullptr;
      }
      m_deviceQueryCooldown.start();
      if (m_deviceQueryQueued) {
        m_deviceQueryQueued = false;
        this->queryActiveDevices();
      }
    });

    auto result = m_deviceQueryProcess->readAllStandardOutput();
    if (result.isEmpty()) return;

    QJsonParseError parseError;
    QJsonDocument   jDoc = QJsonDocument::fromJson(result, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
      qCWarning(logNSHyprland)
          << "Device query process: error parsing json data from hyprctl!";
      return;
    }

    if (!jDoc.isObject()) {
      qCWarning(logNSHyprland)
          << "Device query: hyprland returned invalid json.";
      return;
    }

    QJsonObject jObj = jDoc.object();

    auto it = jObj.find("keyboards");
    if (it == jObj.end()) {
      qCWarning(logNSHyprland)
          << "Device query: hyprctl is missing the 'keyboards' field.";
      return;
    }

    if (auto val = it.value(); val.isArray()) {
      QJsonArray keyboards = val.toArray();
      for (const QJsonValue &value : keyboards) {
        if (!value.isObject()) continue;

        auto kbInfo = value.toObject();
        if (!kbInfo.value("main").toBool(false)) continue;

        auto layoutIdx = kbInfo.value("active_layout_index").toInt(0);
        if (layoutIdx != m_keyboardLayoutIndex) {
          m_keyboardLayoutIndex = layoutIdx;
          emit keyboardLayoutIndexChanged();
        }
        break;
      }
    }
  });
}
} // namespace ns::hyprland
