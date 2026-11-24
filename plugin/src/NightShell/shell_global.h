#pragma once

#include <QGuiApplication>
#include <QJSEngine>
#include <QList>
#include <QObject>
#include <QQmlEngine>
#include <QScreen>
#include <QtQmlIntegration>

namespace ns {
class ShellGlobal : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON
  QML_UNCREATABLE("Must be provided by C++")

  Q_PROPERTY(QList<QScreen *> screens READ screens NOTIFY screensChanged)

public:
  explicit ShellGlobal(QGuiApplication *app, QObject *parent = nullptr);
  ~ShellGlobal() override;
  Q_DISABLE_COPY_MOVE(ShellGlobal)

  static ShellGlobal *instance();

  [[nodiscard]] QList<QScreen *> screens() const;

signals:
  void screensChanged();

private slots:
  void onScreenAdded(QScreen *screen);
  void onScreenRemoved(QScreen *screen);
  void onScreenDestroyed();

private:
  QGuiApplication *m_app;

  QList<QScreen *> m_screens;
};
} // namespace ns
