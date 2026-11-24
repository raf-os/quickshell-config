#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtGlobal>

#include "shell_global.h"

int main(int argc, char *argv[]) {
  QGuiApplication       app(argc, argv);
  QQmlApplicationEngine engine;

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  ns::ShellGlobal shellGlobalSingleton{&app, &app};

  engine.setExternalSingletonInstance("org.nightshell", "ShellGlobal",
                                      &shellGlobalSingleton);

#if QT_VERSION >= QT_VERSION_CHECK(6, 12, 1)
  engine.loadFromModule("ns", "Main");
#else
#ifdef QML_ROOT
  engine.addImportPath(QStringLiteral(QML_ROOT));
  engine.load(QUrl::fromLocalFile(QStringLiteral(QML_ROOT "/Main.qml")));
#else
  engine.loadFromModule("ns", "Main");
#endif
#endif

  return app.exec();
}
