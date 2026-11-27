#pragma once

#include <QList>
#include <QObject>
#include <QString>
#include <QtQmlIntegration>
#include <QtTypes>

class HyprConfigsTest;

namespace ns::utils {
class KeyboardLayoutXMLSerializer;
}

namespace ns::wayland::hyprland {
class Hyprctl;

class HyprKeyboardLayout {
  Q_GADGET
  QML_VALUE_TYPE(hyprKeyboardLayout)

  Q_PROPERTY(QString layout MEMBER layout CONSTANT)
  Q_PROPERTY(QString variant MEMBER variant CONSTANT)
  Q_PROPERTY(QString description MEMBER description CONSTANT)

public:
  QString layout;
  QString variant;
  QString description;

  bool operator==(const HyprKeyboardLayout &other) const {
    return layout == other.layout && variant == other.variant &&
           description == other.description;
  };
};

class HyprConfigs : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QList<ns::wayland::hyprland::HyprKeyboardLayout> keyboardLayouts
                 READ keyboardLayouts NOTIFY keyboardLayoutsChanged)
  Q_PROPERTY(ns::wayland::hyprland::HyprKeyboardLayout activeKeyboardLayout READ
                 activeKeyboardLayout NOTIFY activeKeyboardLayoutChanged)

  friend class ::HyprConfigsTest;

public:
  explicit HyprConfigs(
      Hyprctl                            *hyprctl,
      utils::KeyboardLayoutXMLSerializer *keyboardLayoutXMLSerializer,
      QObject                            *parent = nullptr);

  [[nodiscard]] QList<HyprKeyboardLayout> keyboardLayouts() const;
  [[nodiscard]] HyprKeyboardLayout        activeKeyboardLayout() const;

  void setActiveLayoutIndex(qint64 index);

public slots:
  void queryConfigs();
  void queryDevices();
  void onActiveLayoutChanged(const QString &keyboardName,
                             const QString &layoutName);

signals:
  void keyboardLayoutsChanged();
  void activeKeyboardLayoutChanged();

private:
  Hyprctl                            *m_hyprctl;
  utils::KeyboardLayoutXMLSerializer *m_keyboardXmlSerializer;

  qint64 m_kbdIdx = 0;

  bool m_descriptionsPending = false;
  bool m_descriptionsQueued  = false;

  bool m_devicesPending = false;

  QList<HyprKeyboardLayout> m_keyboardLayouts;

  void processConfigData(const QByteArray &data);
  void processDeviceData(const QByteArray &data);
};
} // namespace ns::wayland::hyprland
