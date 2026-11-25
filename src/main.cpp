#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtGlobal>

#include "compositor_support.h"
#include "shell_global.h"

int main(int argc, char *argv[]) {
  QGuiApplication       app(argc, argv);
  QQmlApplicationEngine engine;

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  ns::wayland::CompositorData compositorData{};

  ns::ShellGlobal shellGlobalSingleton{compositorData.current(), &app, &app};

  engine.setExternalSingletonInstance("org.nightshell", "ShellGlobal",
                                      &shellGlobalSingleton);

  engine.loadFromModule("ns", "Main");

  return app.exec();
}
