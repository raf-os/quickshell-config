#include <qcoreapplication.h>
#include <qdiriterator.h>
#include <qfilesystemwatcher.h>
#include <qguiapplication.h>
#include <qlist.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpointer.h>
#include <qqmlapplicationengine.h>
#include <qqmlcomponent.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qtimer.h>
#include <qurl.h>

#ifndef SOURCE_DIR
#define SOURCE_DIR = "./"
#endif // !SOURCE_DIR

void include_watch_directory(QFileSystemWatcher *fw, const QString &path) {
  QDirIterator it(QString::fromUtf8(SOURCE_DIR) + path,
                  QDirIterator::Subdirectories);
  QList<QString> acceptedFT({"qml", "js"});
  while (it.hasNext()) {
    it.next();
    auto i = it.fileInfo();
    if (i.isFile() && acceptedFT.contains(i.completeSuffix())) {
      fw->addPath(i.absoluteFilePath());
    }
  }
}

void init_live_reload(QQmlApplicationEngine *engine) {
  QFileSystemWatcher *w = new QFileSystemWatcher();
  include_watch_directory(w, "/qml");
  include_watch_directory(w, "/components");

  QUrl mainPath = QUrl::fromLocalFile(SOURCE_DIR + QString("/qml/App.qml"));

  QTimer *debounce = new QTimer();
  debounce->setSingleShot(true);
  debounce->setInterval(100);

  static auto previousItem = QPointer<QQuickItem>(nullptr);

  QObject::connect(w, &QFileSystemWatcher::fileChanged,
                   [w, debounce](const QString &file) {
                     w->addPath(file);
                     debounce->start();
                   });

  QObject::connect(debounce, &QTimer::timeout, [engine, mainPath] {
    qDebug() << "Attempting hot reload...";
    QQuickWindow *wnd = nullptr;
    for (const auto obj : engine->rootObjects()) {
      if (obj->objectName() == "Main") {
        wnd = qobject_cast<QQuickWindow *>(obj);
        break;
      }
    }
    // auto wnd = qobject_cast<QQuickWindow *>(engine->rootObjects().first());
    if (!wnd) {
      qWarning() << "Error casting root engine object to a QQuickWindow, live "
                    "reload failed.";
      return;
    }

    QQmlComponent component(engine, mainPath, wnd);

    if (component.status() == QQmlComponent::Error) {
      qWarning() << "RELOAD FAILED!\n" << component.errorString();
      return;
    }

    auto tempItem = qobject_cast<QQuickItem *>(component.create());

    if (!tempItem) {
      qWarning() << "RELOAD FAILED!\n" << component.errorString();
      return;
    }

    auto children = wnd->contentItem()->childItems();
    if (!children.isEmpty()) {
      for (const auto item : children) {
        if (item->objectName() == "App") {
          item->setParentItem(nullptr);
          if (item == previousItem)
            previousItem = nullptr;
          item->deleteLater();
          break;
        }
      }
    }

    if (previousItem) {
      previousItem->setParentItem(nullptr);
      previousItem->deleteLater();
    }

    engine->clearComponentCache();

    if (tempItem) {
      previousItem = tempItem;
      previousItem->setObjectName("App");
      previousItem->setParentItem(wnd->contentItem());
    }

    qDebug() << "Hot reload successful!";
  });
}

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

#ifdef DEBUG
  engine.load(QUrl::fromLocalFile(SOURCE_DIR + QString("/Main.qml")));
  engine.addImportPath(SOURCE_DIR + QString("/../"));
  init_live_reload(&engine);
#else
  engine.loadFromModule("MyShellControlPanel", "Main");
#endif // DEBUG
  return app.exec();
}
