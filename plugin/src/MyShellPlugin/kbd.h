#pragma once

#include <libxml/xmlstring.h>
#include <libxml/xpath.h>
#include <qcontainerfwd.h>
#include <qmap.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
class KeyboardLayoutHandler : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QVariantMap layouts READ layouts NOTIFY layoutsChanged)
  Q_PROPERTY(QString cachePath READ cachePath WRITE setCachePath NOTIFY
                 cachePathChanged)

public:
  explicit KeyboardLayoutHandler(QObject *parent = nullptr);

  [[nodiscard]] QVariantMap layouts() const;
  [[nodiscard]] QString cachePath() const;
  void setCachePath(const QString &path);

  Q_INVOKABLE void debugPrintLayouts();

signals:
  void layoutsChanged();
  void cachePathChanged();

private:
  QVariantMap m_layouts;
  QString m_cachePath;
  const char *m_evdevPath = "/usr/share/X11/xkb/rules/evdev.xml";

  bool rebuildLayouts();
  void traverseXmlNodes(xmlNodeSetPtr nodes);
};
} // namespace myqmlplugin
