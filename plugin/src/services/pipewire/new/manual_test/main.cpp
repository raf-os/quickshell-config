#include <qcoreapplication.h>
#include <qguiapplication.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlabstracturlinterceptor.h>
#include <qqmlapplicationengine.h>
#include <qurl.h>

int main(int argc, char *argv[]) {
  QGuiApplication       app(argc, argv);
  QQmlApplicationEngine engine;

  engine.addPluginPath(PLUGINS_DIR);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("tests.pipewire_manual", "Main");

  return app.exec();
}
