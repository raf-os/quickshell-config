#include "notification.h"
#include "notificationserver.h"

#include <qcontainerfwd.h>
#include <qdbusargument.h>
#include <qloggingcategory.h>
#include <qmap.h>
#include <qobject.h>
#include <qproperty.h>
#include <qtimer.h>
#include <qtypes.h>
#include <qvariant.h>

namespace ns {
namespace notifications {
Q_DECLARE_LOGGING_CATEGORY(logNSNotifications)

Notification::Notification(quint32  id,
                           QObject *parent)
    : QObject(parent),
      m_id(id) {}

void Notification::expire() { close(NotificationCloseReason::Expired); }
void Notification::dismiss() { close(NotificationCloseReason::Dismissed); }

void Notification::close(NotificationCloseReason::Enum reason) {
  m_closeReason = reason;

  if (reason != 0) {
    NotificationServer::instance()->deleteNotification(this, reason);
  }
}

void Notification::updateProperties(const QString     &appName,
                                    QString            appIcon,
                                    const QString     &summary,
                                    const QString     &body,
                                    const QStringList &actions,
                                    QVariantMap        hints,
                                    qint32             expireTimeout) {
  QScopedPropertyUpdateGroup group;

  m_expireTimeout = expireTimeout;
  m_appName       = appName;
  m_appIcon       = appIcon;
  m_summary       = summary;
  m_body          = body;
  m_desktopEntry  = hints.value("desktop-entry").toString();
  m_urgency = hints.contains("urgency")
                  ? hints.value("urgency").value<NotificationUrgency::Enum>()
                  : NotificationUrgency::Normal;

  m_appIcon = appIcon;

  QString imageDataKey;
  if (hints.contains("image-data"))
    imageDataKey = "image-data";
  else if (hints.contains("image_data"))
    imageDataKey = "image_data";
  else if (hints.contains("icon_data"))
    imageDataKey = "icon_data";

  QString imagePath;

  if (imageDataKey.isEmpty()) {
    m_imageHandler.clear();
  } else {
    auto value = hints.value(imageDataKey).value<QDBusArgument>();
    value >> m_imageHandler.writeImage();
    imagePath = m_imageHandler.urlFor();
  }

  hints.remove("image-data");
  hints.remove("image_data");
  hints.remove("icon_data");

  if (!m_imageHandler.hasData()) {
    QString imagePathName;
    if (hints.contains("image-path"))
      imagePathName = "image-path";
    else if (hints.contains("image_path"))
      imagePathName = "image_path";

    if (!imagePathName.isEmpty()) {
      imagePath = hints.value(imagePathName).value<QString>();

      if (!imagePath.startsWith("file:")) {
        imagePath = "image://qicons/qt/" + imagePath;
      }
    }
  }

  m_imageUrl = imagePath;
  m_hints    = hints;
}

quint32                       Notification::id() const { return m_id; }
NotificationCloseReason::Enum Notification::closeReason() const {
  return m_closeReason;
}
} // namespace notifications
} // namespace ns
