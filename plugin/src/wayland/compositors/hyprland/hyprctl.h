#pragma once

#include <functional>

#include <QByteArray>
#include <QObject>
#include <QString>

namespace ns::wayland::hyprland {
class Hyprctl : public QObject {
  Q_OBJECT

public:
  explicit Hyprctl(QString socketPath, QObject *parent = nullptr);

  void call(QObject *context, const QByteArray &request,
            const std::function<void(bool, QByteArray)> &callback = nullptr);
  void dispatch(const QString &request);
  void reloadConfig();

private:
  const QString m_socketPath;
};
} // namespace ns::wayland::hyprland
