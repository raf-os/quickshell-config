#pragma once

#include <qdbusargument.h>
#include <qdebug.h>
#include <qimage.h>
#include <qlist.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtypes.h>

// Single system tray icon
struct DBusTrayIconPixmap {
  qint32     width  = 0;
  qint32     height = 0;
  QByteArray data;

  [[nodiscard]] QImage createImage() const;

  bool operator==(const DBusTrayIconPixmap &other) const;
};

using DBusTrayIconPixmapList = QList<DBusTrayIconPixmap>;

struct DBusTrayTooltip {
  QString                icon;
  DBusTrayIconPixmapList iconPixmaps;
  QString                title;
  QString                description;

  bool operator==(const DBusTrayTooltip &other) const;
};

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                DBusTrayIconPixmap  &pixmap);
const QDBusArgument &operator<<(QDBusArgument            &argument,
                                const DBusTrayIconPixmap &pixmap);
const QDBusArgument &operator>>(const QDBusArgument    &argument,
                                DBusTrayIconPixmapList &pixmaps);
const QDBusArgument &operator<<(QDBusArgument                &argument,
                                const DBusTrayIconPixmapList &pixmaps);
const QDBusArgument &operator>>(const QDBusArgument &argument,
                                DBusTrayTooltip     &tooltip);
const QDBusArgument &operator<<(QDBusArgument         &argument,
                                const DBusTrayTooltip &tooltip);

QDebug operator<<(QDebug debug, const DBusTrayIconPixmap &pixmap);
QDebug operator<<(QDebug debug, const DBusTrayTooltip &tooltip);
