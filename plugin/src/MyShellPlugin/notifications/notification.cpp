#include "notification.h"

#include <qcontainerfwd.h>
#include <qdbusargument.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qmap.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmllist.h>
#include <qtimer.h>
#include <qtypes.h>
#include <qvariant.h>

#include "notificationserver.h"

namespace ns {
namespace notifications {
Q_DECLARE_LOGGING_CATEGORY(logNSNotifications)

Notification::Notification(quint32 id, QObject *parent)
    : QObject(parent), m_id(id) {}

QQmlListProperty<NotificationAction> Notification::actions() {
  return QQmlListProperty<NotificationAction>(this,
                                              &m_actions,
                                              &Notification::actionListCount,
                                              &Notification::actionListAt);
}

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
  Qt::beginPropertyUpdateGroup();

  m_expireTimeout  = expireTimeout;
  m_appName        = appName;
  m_appIcon        = appIcon;
  m_summary        = summary;
  m_body           = body;
  m_resident       = hints.value("resident").toBool();
  m_hasActionIcons = hints.value("action-icons").toBool();
  m_desktopEntry   = hints.value("desktop-entry").toString();
  m_urgency = hints.contains("urgency")
                  ? hints.value("urgency").value<NotificationUrgency::Enum>()
                  : NotificationUrgency::Normal;

  m_appIcon = appIcon;

  QString imageDataKey;
  if (hints.contains("image-data")) imageDataKey = "image-data";
  else if (hints.contains("image_data")) imageDataKey = "image_data";
  else if (hints.contains("icon_data")) imageDataKey = "icon_data";

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
    if (hints.contains("image-path")) imagePathName = "image-path";
    else if (hints.contains("image_path")) imagePathName = "image_path";

    if (!imagePathName.isEmpty()) {
      imagePath = hints.value(imagePathName).value<QString>();

      if (!imagePath.startsWith("file:")) {
        imagePath = "image://qicons/qt/" + imagePath;
      }
    }
  }

  m_imageUrl = imagePath;
  m_hints    = hints;

  bool actionsChangedFlag = false;
  auto deletedActions     = QList<NotificationAction *>();

  if (actions.length() % 2 == 0) {
    int actionIndex = 0;
    for (auto i = 0; i != actions.length(); i += 2) {
      const auto &identifier = actions.at(i);
      const auto &text       = actions.at(i + 1);

      auto *existingAction = actionIndex < m_actions.length()
                                 ? m_actions.at(actionIndex)
                                 : nullptr;

      if (existingAction && identifier == existingAction->identifier()) {
        existingAction->setText(text);
      } else {
        auto *newAction = new NotificationAction(identifier, text, this);

        if (existingAction) {
          deletedActions.append(existingAction);
          m_actions.replace(actionIndex, newAction);
        } else {
          m_actions.append(newAction);
        }

        actionsChangedFlag = true;
      }

      actionIndex++;
    }

    for (auto i = actionIndex; i < m_actions.length() - 1; ++i) {
      deletedActions.append(m_actions.at(i));
      m_actions.remove(i);
      actionsChangedFlag = true;
    }
  } else {
    qCWarning(logNSNotifications)
        << this << "(" << appName << ") sent an invalid action set length.";
  }

  Qt::endPropertyUpdateGroup();

  if (actionsChangedFlag) emit actionsChanged();

  for (auto *action : deletedActions) {
    action->deleteLater();
  }
}

quint32                       Notification::id() const { return m_id; }
NotificationCloseReason::Enum Notification::closeReason() const {
  return m_closeReason;
}

QString NotificationAction::identifier() const { return m_identifier; }
QString NotificationAction::text() const { return m_text; }
void    NotificationAction::setText(const QString &text) {
  if (m_text == text) return;

  m_text = text;
  emit textChanged();
}

void NotificationAction::invoke() {
  NotificationServer::instance()->ActionInvoked(m_notification->id(),
                                                m_identifier);

  if (m_notification->bindableResident().value() == false) {
    m_notification->close(NotificationCloseReason::Dismissed);
  }
}
} // namespace notifications
} // namespace ns
