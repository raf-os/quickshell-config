#include <memory>

#include <print>
#include <qbuffer.h>
#include <qcontainerfwd.h>
#include <qdbusargument.h>
#include <qdbusconnection.h>
#include <qdbusmetatype.h>
#include <qdir.h>
#include <qimage.h>
#include <qmap.h>
#include <qstringview.h>
#include <qvariant.h>

#include "nightshell_notification_client.h"
#include "types.h"

org::freedesktop::Notifications *DBUS_INTERFACE = nullptr;
#ifdef ICON_TEST_PATH
QDir sourceDir(ICON_TEST_PATH);
#else
QDir sourceDir();
#endif

QHash<quint32, std::shared_ptr<DBusNotification>> notifications;

const QDBusArgument &operator>>(const QDBusArgument   &argument,
                                DBusNotificationImage &image) {
  argument.beginStructure();
  argument >> image.width;
  argument >> image.height;
  argument >> image.rowstride;
  argument >> image.has_alpha;
  auto sampleBits = qdbus_cast<qint32>(argument); // should be 8
  argument >> image.channels;
  argument >> image.data;
  argument.endStructure();

  return argument;
}

const QDBusArgument &operator<<(QDBusArgument               &argument,
                                const DBusNotificationImage &image) {
  argument.beginStructure();
  argument << image.width;
  argument << image.height;
  argument << image.width * (image.has_alpha ? 4 : 3);
  argument << image.has_alpha;
  argument << 8;
  argument << (image.has_alpha ? 4 : 3);
  argument << image.data;
  argument.endStructure();

  return argument;
}

int main(int   argc,
         char *argv[]) {
  qDBusRegisterMetaType<DBusNotificationImage>();

  auto dbus_connection = QDBusConnection::sessionBus();
  if (!dbus_connection.isConnected()) {
    std::println("Unable to connect to DBUS service.");
  }

  DBUS_INTERFACE =
      new org::freedesktop::Notifications("org.freedesktop.Notifications",
                                          "/org/freedesktop/Notifications",
                                          dbus_connection);

  DBusServerInformation serverInformation(
      DBUS_INTERFACE->GetServerInformation());

  std::println("Server name: {}"
               "\nServer vendor: {}"
               "\nServer version: {}"
               "\nSpec version: {}",
               serverInformation.name,
               serverInformation.vendor,
               serverInformation.version,
               serverInformation.spec);

  DBusServerCapabilities capabilites(DBUS_INTERFACE->GetCapabilities());

  if (!capabilites.icon_static) {
    std::println(
        "DBUS notification server does not have 'icon-static' capabilites.");
    return 1;
  }

  auto  fname = sourceDir.filePath("testimg.png");
  QFile iconFile(fname);

  if (!iconFile.exists()) {
    std::println("Missing test image.");
    return 1;
  }

  if (!iconFile.open(QIODevice::ReadOnly)) {
    std::println("Error opening icon image for reading.");
    return 1;
  }

  QByteArray iconData = iconFile.readAll();
  iconFile.close();

  QImage iconPixmap;
  auto   succ = iconPixmap.loadFromData(iconData);

  if (!succ) {
    std::println("Invalid test image.");
    return 1;
  }

  DBusNotificationImage notifImage;
  notifImage.width  = iconPixmap.width();
  notifImage.height = iconPixmap.height();
  notifImage.rowstride =
      iconPixmap.width() * (iconPixmap.hasAlphaChannel() ? 4 : 3);
  notifImage.has_alpha = iconPixmap.hasAlphaChannel();
  notifImage.channels  = iconPixmap.hasAlphaChannel() ? 4 : 3;
  notifImage.data =
      QByteArray(reinterpret_cast<const char *>(iconPixmap.constBits()),
                 iconPixmap.sizeInBytes());

  QDBusArgument imageArgument;
  imageArgument << notifImage;

  QVariantMap hints;
  hints.insert("image-data", QVariant::fromValue(imageArgument));

  DBusNotificationHints notificationHints{};

  DBusNotification notificationData{.app_name    = "Notification test",
                                    .replaces_id = 0,
                                    .app_icon    = "",
                                    .summary     = "Test",
                                    .body        = "Body",
                                    .actions     = {},
                                    .hints       = std::move(hints)};

  auto notif = DBUS_INTERFACE->Notify("Notification test",
                                      0,
                                      "",
                                      "Test",
                                      "Body",
                                      {"action1", "Action"},
                                      hints,
                                      0);

  std::println("Notification id: {}", notif.value());

  return 0;
}
