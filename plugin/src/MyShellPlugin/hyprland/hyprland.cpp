#include "hyprland.h"

#include <qcontainerfwd.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qprocess.h>

#include "hyprevents.h"

namespace ns::hyprland {
Q_LOGGING_CATEGORY(logNSHyprland,
                   "nightshell.hyprland")

Hyprland::Hyprland(QObject *parent)
    : QObject(parent),
      m_eventHandler(new myqmlplugin::HyprEvents(this)),
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
}

myqmlplugin::HyprEvents *Hyprland::eventHandler() { return m_eventHandler; }

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

  if (m_hyprInputQueryQueued) {
    m_hyprInputQueryQueued = false;
    this->queryHyprInputConfigs();
  }
}
} // namespace ns::hyprland
