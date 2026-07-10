#pragma once

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::hyprland {
class HyprInputConfig : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString kbModel READ default WRITE default NOTIFY kbModelChanged
                 BINDABLE b_kbModel)
  Q_PROPERTY(QString kbOptions READ default WRITE default NOTIFY
                 kbOptionsChanged BINDABLE b_kbOptions)
  Q_PROPERTY(QString kbRules READ default WRITE default NOTIFY kbRulesChanged
                 BINDABLE b_kbRules)

public:
  HyprInputConfig(QObject *parent = nullptr);

  [[nodiscard]] QBindable<QString> b_kbModel() const { return &m_kbModel; }
  [[nodiscard]] QBindable<QString> b_kbOptions() const { return &m_kbOptions; }

signals:
  void kbModelChanged();
  void kbOptionsChanged();
  void kbRulesChanged();

private:
  Q_OBJECT_BINDABLE_PROPERTY(ns::hyprland::HyprInputConfig,
                             QString,
                             m_kbModel,
                             &ns::hyprland::HyprInputConfig::kbModelChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::hyprland::HyprInputConfig,
                             QString,
                             m_kbOptions,
                             &ns::hyprland::HyprInputConfig::kbOptionsChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::hyprland::HyprInputConfig,
                             QString,
                             m_kbRules,
                             &ns::hyprland::HyprInputConfig::kbRulesChanged)
};
} // namespace ns::hyprland
