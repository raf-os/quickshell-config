#pragma once

#include <QGuiApplication>
#include <QJSEngine>
#include <QList>
#include <QMap>
#include <QObject>
#include <QQmlEngine>
#include <QScreen>
#include <QtQmlIntegration>

#include "compositor_support.h"
#include "screen_info.h"

namespace ns::wayland::workspaces {
class WorkspaceManager;
}

namespace ns {
namespace WlCompositorQML {
Q_NAMESPACE
QML_NAMED_ELEMENT(WlCompositor)
QML_FOREIGN_NAMESPACE(ns::wayland::Compositor)
}; // namespace WlCompositorQML

class ShellGlobal : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON
  QML_UNCREATABLE("Must be provided by C++")

  Q_PROPERTY(QList<ns::ScreenInfo *> screens READ screens NOTIFY screensChanged)
  Q_PROPERTY(ns::wayland::Compositor::Enum compositor READ compositor CONSTANT)

public:
  explicit ShellGlobal(ns::wayland::CompositorData compositor,
                       QGuiApplication *app, QObject *parent = nullptr);
  ~ShellGlobal() override;
  Q_DISABLE_COPY_MOVE(ShellGlobal)

  static ShellGlobal *instance();

  [[nodiscard]] QList<ScreenInfo *>          screens() const;
  [[nodiscard]] ns::wayland::CompositorData *compositor();

  ns::wayland::workspaces::WorkspaceManager *workspaceManager();

signals:
  void screensChanged();

private slots:
  void onScreenAdded(QScreen *screen);
  void onScreenRemoved(QScreen *screen);
  void onScreenDestroyed();

private:
  QGuiApplication *m_app;

  QMap<QScreen *, ScreenInfo *> m_screenMap;
  ns::wayland::CompositorData   m_compositor;

  ns::wayland::workspaces::WorkspaceManager *m_workspaceManager = nullptr;
};
} // namespace ns
