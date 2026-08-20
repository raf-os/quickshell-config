#pragma once

#include <string>
#include <utility>

#include <qcontainerfwd.h>
#include <qdbusargument.h>
#include <qdbuspendingreply.h>
#include <qlist.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtypes.h>
#include <qvariant.h>

using QStringPair = QPair<QString, QString>;

struct DBusServerInformation {
  std::string name;
  std::string vendor;
  std::string version;
  std::string spec;

  explicit DBusServerInformation() = delete;
  explicit DBusServerInformation(QDBusPendingReply<QString,
                                                   QString,
                                                   QString,
                                                   QString> &&other) {
    if (!other.isFinished()) other.waitForFinished();
    name    = other.argumentAt<0>().toStdString();
    vendor  = other.argumentAt<1>().toStdString();
    version = other.argumentAt<2>().toStdString();
    spec    = other.argumentAt<3>().toStdString();
  }
};

struct DBusServerCapabilities {
  bool action_icons;
  bool actions;
  bool body;
  bool body_hyperlinks;
  bool body_images;
  bool body_markup;
  bool icon_multi;
  bool icon_static;
  bool persistence;
  bool sound;

  explicit DBusServerCapabilities() {}
  explicit DBusServerCapabilities(QDBusPendingReply<QList<QString>> &&reply) {
    if (!reply.isFinished()) reply.waitForFinished();
    auto v = reply.argumentAt<0>();

    this->fromReply(v);
  }

  void fromReply(QList<QString> &reply) {
    action_icons    = reply.contains("action-icons");
    actions         = reply.contains("actions");
    body            = reply.contains("body");
    body_hyperlinks = reply.contains("body-hyperlinks");
    body_images     = reply.contains("body-images");
    body_markup     = reply.contains("body-markup");
    icon_multi      = reply.contains("icon-multi");
    icon_static     = reply.contains("icon-static");
    persistence     = reply.contains("persistence");
    sound           = reply.contains("sound");
  }
};

struct DBusNotificationImage {
  qint32     width;
  qint32     height;
  qint32     rowstride;
  bool       has_alpha;
  qint32     channels;
  QByteArray data;
};

const QDBusArgument &operator>>(const QDBusArgument   &argument,
                                DBusNotificationImage &image);
const QDBusArgument &operator<<(QDBusArgument               &argument,
                                const DBusNotificationImage &image);

enum class NotificationUrgency : char { Low = 0, Normal = 1, Critical = 2 };

struct DBusNotificationHints {
  bool                   action_icons;
  bool                   resident;
  bool                   transient;
  bool                   suppress_sound;
  NotificationUrgency    urgency = NotificationUrgency::Normal;
  QString                category;
  QString                desktop_entry;
  QString                sound_file;
  QString                sound_name;
  QString                image_path;
  DBusNotificationImage *image_data = nullptr;

  QVariantMap toVariantMap() {
    QVariantMap vm;
    vm.insert("action-icons", action_icons);
    vm.insert("category", category);
    vm.insert("desktop-entry", desktop_entry);
    vm.insert("image-path", image_path);
    vm.insert("resident", resident);
    vm.insert("sound-file", sound_file);
    vm.insert("sound-name", sound_name);
    vm.insert("suppress-sound", suppress_sound);
    vm.insert("transient", transient);
    vm.insert("urgency", static_cast<char>(urgency));

    if (image_data) {
      QDBusArgument db;
      db << image_data;
      vm.insert("image-data", QVariant::fromValue(db));
    }

    return std::move(vm);
  }
};

struct DBusNotificationAction {
  QString identifier;
  QString label;
};

struct DBusNotification {
  QString        app_name;
  quint32        replaces_id;
  QString        app_icon;
  QString        summary;
  QString        body;
  QList<QString> actions;
  QVariantMap    hints;

  QVariantList getArguments() {
    QVariantList l;
    l << QVariant::fromValue(app_name);
    l << QVariant::fromValue(replaces_id);
    l << QVariant::fromValue(app_icon);
    l << QVariant::fromValue(summary);
    l << QVariant::fromValue(body);
    l << QVariant::fromValue(actions);
    l << QVariant::fromValue(hints);
    return l;
  }
};
