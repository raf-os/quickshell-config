#pragma once

#include <QList>
#include <QObject>
#include <QString>
#include <QtQmlIntegration>

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

  friend class ::HyprConfigsTest;

public:
  explicit HyprConfigs(
      Hyprctl                            *hyprctl,
      utils::KeyboardLayoutXMLSerializer *keyboardLayoutXMLSerializer,
      QObject                            *parent = nullptr);

  [[nodiscard]] QList<HyprKeyboardLayout> keyboardLayouts() const;

public slots:
  void queryConfigs();

signals:
  void keyboardLayoutsChanged();

private:
  Hyprctl                            *m_hyprctl;
  utils::KeyboardLayoutXMLSerializer *m_keyboardXmlSerializer;

  bool m_pending = false;
  bool m_queued  = false;

  QList<HyprKeyboardLayout> m_keyboardLayouts;

  void processConfigData(const QByteArray &data);
};
} // namespace ns::wayland::hyprland
