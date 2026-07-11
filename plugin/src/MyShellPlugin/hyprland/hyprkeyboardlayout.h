#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::hyprland {
class HyprKeyboardLayout : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString layout READ layout CONSTANT)
  Q_PROPERTY(QString variant READ variant CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)

public:
  explicit HyprKeyboardLayout(const QString &layout,
                              const QString &variant,
                              const QString &description,
                              QObject       *parent = nullptr);

  [[nodiscard]] QString layout() const;
  [[nodiscard]] QString variant() const;
  [[nodiscard]] QString description() const;

private:
  QString m_layout;
  QString m_variant;
  QString m_description;
};
} // namespace ns::hyprland
