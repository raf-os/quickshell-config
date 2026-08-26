#include "statusnotifieritem.h"

#include <qdbusconnection.h>
#include <qdbuserror.h>
#include <qdbusmetatype.h>
#include <qicon.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qproperty.h>
#include <qsize.h>

#include "dbus_item.h"
#include "dbusimage.h"
#include "dbustypes.h"

namespace ns::systemtray {
Q_LOGGING_CATEGORY(logNSStatusNotifierItem, "ns.systemtray.StatusNotifierItem")

namespace Status {
Status::Enum fromString(const QString &value) {
  if (value == "Passive") return Passive;
  if (value == "Active") return Active;
  if (value == "NeedsAttention") return NeedsAttention;

  qCWarning(logNSStatusNotifierItem)
      << "Received invalid status" << value << "from StatusNotifierItem.";
  return Passive;
}
} // namespace Status

namespace Category {
Category::Enum fromString(const QString &value) {
  if (value == "Hardware") return Hardware;
  if (value == "SystemServices") return SystemServices;
  if (value == "ApplicationStatus") return ApplicationStatus;
  if (value == "Communications") return Communications;

  qCWarning(logNSStatusNotifierItem)
      << "Received invalid category" << value << "from StatusNotifierItem.";
  return ApplicationStatus;
}
} // namespace Category

StatusNotifierItem::StatusNotifierItem(const QString &address, QObject *parent)
    : QObject(parent), m_watcherId(address) {
  qDBusRegisterMetaType<DBusTrayIconPixmap>();
  qDBusRegisterMetaType<DBusTrayIconPixmapList>();
  qDBusRegisterMetaType<DBusTrayTooltip>();

  auto splitIdx = address.indexOf("/");
  auto conn     = splitIdx == -1 ? address : address.sliced(0, splitIdx);
  auto path = splitIdx == -1 ? "/StatusNotifierItem" : address.sliced(splitIdx);

  m_item = new QDBusStatusNotifierItem(
      conn, path, QDBusConnection::sessionBus(), this);

  if (!m_item->isValid()) {
    qCWarning(logNSStatusNotifierItem)
        << "Unable to create item for connection" << conn;
    return;
  }

  QObject::connect(m_item, &QDBusStatusNotifierItem::NewTitle, this, [this]() {
    b_title = m_item->title();
  });

  QObject::connect(
      m_item,
      &QDBusStatusNotifierItem::NewStatus,
      this,
      [this](const QString &status) { b_status = Status::fromString(status); });

  QObject::connect(m_item,
                   &QDBusStatusNotifierItem::NewToolTip,
                   this,
                   [this]() { this->readTooltip(); });

  b_hasMenu.setBinding([this]() {
    // Apparently this is a KDE specific thing
    return !b_menuPath.value().path().isEmpty() &&
           b_menuPath.value().path() != "/NO_DBUSMENU";
  });

  readAllParameters();

  m_isReady = true;
  emit ready();
}

void StatusNotifierItem::readAllParameters() {
  if (m_item == nullptr) return;

  QScopedPropertyUpdateGroup scopeGuard;
  b_id                = m_item->id();
  b_title             = m_item->title();
  b_status            = Status::fromString(m_item->status());
  b_category          = Category::fromString(m_item->category());
  b_windowId          = m_item->windowId();
  b_iconThemePath     = m_item->iconThemePath();
  b_iconName          = m_item->iconName();
  b_attentionIconName = m_item->attentionIconName();
  b_overlayIconName   = m_item->overlayIconName();
  b_isMenuOnly        = m_item->itemIsMenu();

  this->readTooltip();
  this->readIconData();

  b_menuPath = m_item->menu();
}

void StatusNotifierItem::readTooltip() {
  if (!m_item) return;

  QScopedPropertyUpdateGroup scope;
  b_tooltipTitle       = m_item->toolTip().title;
  b_tooltipDescription = m_item->toolTip().description;
}

void StatusNotifierItem::readIconData() {
  if (!m_item) return;

  QScopedPropertyUpdateGroup scope;
  m_pixmapList          = m_item->iconPixmap();
  m_attentionPixmapList = m_item->attentionIconPixmap();
  m_overlayPixmapList   = m_item->overlayIconPixmap();
}

bool StatusNotifierItem::isValid() const { return m_item->isValid(); }
bool StatusNotifierItem::isReady() const { return m_isReady; }

void StatusNotifierItem::createPixmap(const QSize &size) {
  auto needsAttention = b_status.value() == Status::NeedsAttention;

  auto getClosestPixmap = [](const QSize            &size,
                             DBusTrayIconPixmapList &pixmaps) {
    DBusTrayIconPixmap *ret = nullptr;

    for (auto &pixmap : pixmaps) {
      if (ret == nullptr) {
        ret = &pixmap;
        continue;
      }

      auto existingAdequate =
          ret->width >= size.width() && ret->height >= size.height();
      auto newAdequate =
          pixmap.width >= size.width() && pixmap.height >= size.height();
      auto newSmaller =
          pixmap.width < ret->width || pixmap.height < ret->height;

      if ((existingAdequate && newAdequate && newSmaller) ||
          (!existingAdequate && !newSmaller))
      {
        ret = &pixmap;
      }
    }

    return ret;
  };

  dbusprovider::ImageHandleAdapter imageAdapter;

  if (needsAttention) {
    if (!b_attentionIconName.value().isEmpty()) {
      b_iconUrl = "image://qicons/qt/" + b_attentionIconName.value();
      return;
    } else {
      auto *icon = getClosestPixmap(size, m_attentionPixmapList);

      if (icon == nullptr) {
        // Fall back to regular icon if it fails to get an attention icon
        icon = getClosestPixmap(size, m_pixmapList);
      }

      if (icon != nullptr) {
        imageAdapter.data   = &icon->data;
        imageAdapter.width  = icon->width;
        imageAdapter.height = icon->height;
      }
    }
  } else {
    if (!b_iconName.value().isEmpty()) {
      b_iconUrl = "image://qicons/qt/" + b_iconName.value();
    } else {
      auto *icon = getClosestPixmap(size, m_pixmapList);

      if (icon != nullptr) {
        imageAdapter.data   = &icon->data;
        imageAdapter.width  = icon->width;
        imageAdapter.height = icon->height;
      }
    }
  }
}
} // namespace ns::systemtray
