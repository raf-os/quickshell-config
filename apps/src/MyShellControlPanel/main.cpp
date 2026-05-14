#include <qbuffer.h>
#include <qcommandlineoption.h>
#include <qcommandlineparser.h>
#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qdir.h>
#include <qdiriterator.h>
#include <qfilesystemwatcher.h>
#include <qguiapplication.h>
#include <qlist.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpointer.h>
#include <qqmlabstracturlinterceptor.h>
#include <qqmlapplicationengine.h>
#include <qqmlcomponent.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qtimer.h>
#include <qurl.h>

#ifndef SOURCE_DIR
#define SOURCE_DIR = "./"
#endif // !SOURCE_DIR

#ifdef DEBUG
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

/**
 * Adapted from
 * https://github.com/gyroflow/gyroflow/blob/master/src/ui_live_reload.cpp
 */
void init_live_reload(QQmlApplicationEngine *engine,
                      const QString &initialPage) {
  QFileSystemWatcher *w = new QFileSystemWatcher();
  include_watch_directory(w, "");

  QUrl mainPath = QUrl::fromLocalFile(SOURCE_DIR + QString("/App.qml"));

  QTimer *debounce = new QTimer();
  debounce->setSingleShot(true);
  debounce->setInterval(200);

  static auto previousItem = QPointer<QQuickItem>(nullptr);

  QObject::connect(
      w, &QFileSystemWatcher::fileChanged, [w, debounce](const QString &file) {
        debounce->start();
        QTimer::singleShot(50, [w, debounce, file] { w->addPath(file); });
      });

  QObject::connect(
      debounce, &QTimer::timeout, [engine, w, mainPath, initialPage] {
        qDebug() << "Attempting hot reload...";
        QQuickWindow *wnd = nullptr;
        for (const auto obj : engine->rootObjects()) {
          if (obj->objectName() == "Main") {
            wnd = qobject_cast<QQuickWindow *>(obj);
            break;
          }
        }
        // auto wnd = qobject_cast<QQuickWindow
        // *>(engine->rootObjects().first());
        if (!wnd) {
          qWarning()
              << "Error casting root engine object to a QQuickWindow, live "
                 "reload failed.";
          return;
        }

        QQmlComponent component(engine, mainPath, wnd);

        if (component.status() == QQmlComponent::Error) {
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
            }
          }
        }

        if (previousItem) {
          previousItem->setParentItem(nullptr);
          previousItem->deleteLater();
        }

        engine->clearComponentCache();

        auto tempItem =
            qobject_cast<QQuickItem *>(component.createWithInitialProperties(
                {{"desiredInitialPath", initialPage}}));

        if (!tempItem) {
          qWarning() << "RELOAD FAILED!\n" << component.errorString();
          return;
        }

        if (tempItem) {
          previousItem = tempItem;
          previousItem->setObjectName("App");
          previousItem->setParentItem(wnd->contentItem());
        }

        qDebug() << "Hot reload successful!";
      });
}

void iterateDirQml(const QDir &dir, QStringList *prefixList) {
  auto dirName = dir.dirName();
  prefixList->append(dirName);
  QStringList moduleLines;
  QDirIterator it(dir.absolutePath(),
                  QDir::Dirs | QDir::Files | QDir::NoDot | QDir::NoDotAndDotDot,
                  QDirIterator::NoIteratorFlags);
  while (it.hasNext()) {
    it.next();
    if (it.fileInfo().isFile()) {
      if (it.fileInfo().suffix() != "qml")
        continue;
      QString name = it.fileInfo().baseName();
      if (dirName == "singletons")
        name.prepend("singleton ");
      moduleLines << name + " 1.0 " + it.fileName();
    } else if (it.fileInfo().isDir()) {
      QStringList newPrefixList(*prefixList);
      iterateDirQml(QDir(it.fileInfo().absoluteFilePath()), &newPrefixList);
    }
  }

  if (moduleLines.length() > 0) {
    QFile file(dir.absolutePath() + "/qmldir");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qWarning() << "Unable to open qmldir file for writing. Dev mode is not "
                    "going to work as expected.";
      return;
    }

    QTextStream out(&file);
    out << "module " << prefixList->join(".") << "\n";
    out << moduleLines.join("\n");

    file.close();
  }
}

class DevUrlInterceptor : public QQmlAbstractUrlInterceptor {
public:
  QUrl intercept(const QUrl &url, DataType type) override {
    if (url.scheme() == "qrc") {
      QString sourcePath = url.path();
      if (sourcePath.startsWith("/qt/qml/MyShellControlPanel/singletons/")) {
        const auto sidx = sourcePath.indexOf("/singletons/");
        sourcePath.slice(sidx);
        QUrl fileUrl = QUrl::fromLocalFile(SOURCE_DIR + sourcePath);
        if (QFile::exists(fileUrl.toLocalFile())) {
          return fileUrl;
        }
      }
    }
    return url;
  }
};
#endif // DEBUG

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QCommandLineParser parser;
  parser.addHelpOption();

  QCommandLineOption initialPageOption("p", "Default app page.", "path");
  parser.addOption(initialPageOption);

  parser.process(app);

  QString initialPage = parser.value(initialPageOption);

  QQmlApplicationEngine engine;

#ifdef DEBUG
  engine.addUrlInterceptor(new DevUrlInterceptor);
#endif // DEBUG

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  QObject::connect(&engine, &QQmlApplicationEngine::quit, &app,
                   &QGuiApplication::quit);

  engine.setInitialProperties({{"desiredInitialPath", initialPage}});

#ifdef DEBUG
  QStringList prefixList;
  iterateDirQml(QDir(SOURCE_DIR), &prefixList);
  engine.addImportPath(SOURCE_DIR);
  engine.load(QUrl::fromLocalFile(SOURCE_DIR + QString("/Main.qml")));
  init_live_reload(&engine, initialPage);
#else
  engine.loadFromModule("MyShellControlPanel", "Main");
#endif // DEBUG
  return app.exec();
}
