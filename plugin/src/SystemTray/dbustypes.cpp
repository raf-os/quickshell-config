#include "dbustypes.h"

#include <qdbusargument.h>
#include <qendian.h>
#include <qimage.h>
#include <qsysinfo.h>
#include <qtypes.h>

QImage DBusTrayIconPixmap::createImage() const {
  // https://www.freedesktop.org/wiki/Specifications/StatusNotifierItem/Icons/
  // "The data is represented in ARGB32 format and is in the network byte order"
  //
  // Thus, conversion to little endian could be necessary. This is the case for
  // the x86 architecture.
  if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
    auto       *newbuf = new quint32[this->data.size()];
    const auto *oldbuf =
        reinterpret_cast<const quint32 *>(this->data.constData());

    for (uint i = 0; i < this->data.size() / sizeof(quint32); ++i) {
      newbuf[i] = qFromBigEndian(oldbuf[i]);
    }

    return QImage(
        reinterpret_cast<const uchar *>(newbuf),
        this->width,
        this->height,
        QImage::Format_ARGB32,
        [](void *ptr) { delete[] reinterpret_cast<quint32 *>(ptr); },
        newbuf);
  } else {
    return QImage(reinterpret_cast<const uchar *>(this->data.constData()),
                  this->width,
                  this->height,
                  QImage::Format_ARGB32);
  }
}

bool DBusTrayIconPixmap::operator==(const DBusTrayIconPixmap &other) const {
  return this->width == other.width && this->height == other.height &&
         this->data == other.data;
}

bool DBusTrayTooltip::operator==(const DBusTrayTooltip &other) const {
  return icon == other.icon && title == other.title &&
         description == other.description && iconPixmaps == other.iconPixmaps;
}

QDebug operator<<(QDebug debug, const DBusTrayIconPixmap &pixmap) {
  debug.nospace() << "DBusTrayIconPixmap(width=" << pixmap.width
                  << ", height=" << pixmap.height << ")";
  return debug;
}

QDebug operator<<(QDebug debug, const DBusTrayTooltip &tooltip) {
  debug.nospace() << "DBusTrayTooltip(title=" << tooltip.title
                  << ", description=" << tooltip.description
                  << ", icon=" << tooltip.icon
                  << ", iconPixmaps=" << tooltip.iconPixmaps << ")";

  return debug;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                DBusTrayIconPixmap  &pixmap) {
  argument.beginStructure();
  argument >> pixmap.width;
  argument >> pixmap.height;
  argument >> pixmap.data;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator<<(QDBusArgument            &argument,
                                const DBusTrayIconPixmap &pixmap) {
  argument.beginStructure();
  argument << pixmap.width;
  argument << pixmap.height;
  argument << pixmap.data;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument    &argument,
                                DBusTrayIconPixmapList &pixmaps) {
  argument.beginArray();
  pixmaps.clear();

  while (!argument.atEnd()) {
    pixmaps.append(qdbus_cast<DBusTrayIconPixmap>(argument));
  }

  argument.endArray();
  return argument;
}

const QDBusArgument &operator<<(QDBusArgument                &argument,
                                const DBusTrayIconPixmapList &pixmaps) {
  argument.beginArray(qMetaTypeId<DBusTrayIconPixmap>());

  for (const auto &pixmap : pixmaps) {
    argument << pixmap;
  }

  argument.endArray();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                DBusTrayTooltip     &tooltip) {
  argument.beginStructure();
  argument >> tooltip.icon;
  argument >> tooltip.iconPixmaps;
  argument >> tooltip.title;
  argument >> tooltip.description;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator<<(QDBusArgument         &argument,
                                const DBusTrayTooltip &tooltip) {
  argument.beginStructure();
  argument << tooltip.icon;
  argument << tooltip.iconPixmaps;
  argument << tooltip.title;
  argument << tooltip.description;
  argument.endStructure();
  return argument;
}
