#include "statusnotifieritem.h"

#include <memory>

#include <QtCore>
#include <qdbusconnection.h>
#include <qdbuserror.h>
#include <qdbusmetatype.h>
#include <qicon.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qproperty.h>
#include <qsize.h>

#include "dbus_item.h"
#include "dbusmenuhandle.h"
#include "dbustypes.h"
#include "iconprovider.h"
#include "trayimagehandle.h"

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
    : QObject(parent), m_watcherId(address),
      m_imageHandle(std::make_unique<TrayImageHandle>(this)) {
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

  QObject::connect(m_item, &QDBusStatusNotifierItem::NewTitle, this,
      [this]() { b_title = m_item->title(); });

  QObject::connect(m_item, &QDBusStatusNotifierItem::NewStatus, this,
      [this](const QString &status) { b_status = Status::fromString(status); });

  QObject::connect(m_item, &QDBusStatusNotifierItem::NewToolTip, this,
      [this]() { this->readTooltip(); });

  QObject::connect(m_item, &QDBusStatusNotifierItem::NewIcon, this,
      [this]() { this->readIconData(); });
  QObject::connect(m_item, &QDBusStatusNotifierItem::NewAttentionIcon, this,
      [this]() { this->readIconData(); });
  QObject::connect(m_item, &QDBusStatusNotifierItem::NewOverlayIcon, this,
      [this]() { this->readIconData(); });

  b_hasMenu.setBinding([this]() {
    // Apparently this is a KDE specific thing
    return !b_menuPath.value().path().isEmpty() &&
           b_menuPath.value().path() != "/NO_DBUSMENU";
  });

  b_iconUrl.setBinding([this]() -> QString {
    // The image handle has cache busting behavior, so this early return
    // re-routes the pixmap creation logic to the regular icon image provider
    if (b_status.value() == Status::NeedsAttention) {
      auto name = b_attentionIconName.value();
      if (!name.isEmpty())
        return iconprovider::IconImageProvider::getSystemIconRequestString(
            name, b_iconThemePath.value(), {});
    } else {
      auto name = b_iconName.value();
      if (!name.isEmpty() && b_overlayIconName.value().isEmpty()) {
        return iconprovider::IconImageProvider::getSystemIconRequestString(
            name, b_iconThemePath.value(), {});
      }
    }

    if (m_imageHandle) {
      if (!b_iconName.value().isEmpty() && !b_overlayIconName.value().isEmpty())
      {
        // Hopefully this makes better use of qt's image caching behavior so
        // regular icon pixmaps are re-used if their icon/overlay combination is
        // the same as a previous one
        return m_imageHandle->urlFor() % "/?icon=" % b_iconName.value() %
               "&overlay=" % b_overlayIconName.value();
      }

      // Cache busting behavior - will force the generation of a new pixmap.
      // Even if images are not being cached, the url change that's passed to a
      // QML Image component will cause the engine to fetch a new QPixmap.
      return m_imageHandle->urlFor() % "/" %
             QString::number(b_pixmapIndex.value());
    } else return QString();
  });

  readAllParameters();

  b_pixmapList.onValueChanged([this] { this->refreshPixmap(); });
  b_attentionPixmapList.onValueChanged([this] { this->refreshPixmap(); });
  b_overlayPixmapList.onValueChanged([this] { this->refreshPixmap(); });

  auto onMenuPathChanged = [this]() {
    QString path = b_menuPath.value().path();
    if (!b_hasMenu.value()) path = "";
    m_menuHandle.setAddress(m_item->service(), path);
  };

  b_menuPath.onValueChanged(onMenuPathChanged);
  onMenuPathChanged();

  m_isReady = true;

  emit readied();
}

StatusNotifierItem::~StatusNotifierItem() = default;

dbusmenu::DBusMenuHandle *StatusNotifierItem::menuHandle() {
  return &m_menuHandle;
}

void StatusNotifierItem::readAllParameters() {
  if (m_item == nullptr) return;

  QScopedPropertyUpdateGroup scopeGuard;
  b_id            = m_item->id();
  b_title         = m_item->title();
  b_status        = Status::fromString(m_item->status());
  b_category      = Category::fromString(m_item->category());
  b_windowId      = m_item->windowId();
  b_iconThemePath = m_item->iconThemePath();
  b_isMenuOnly    = m_item->itemIsMenu();

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
  b_iconName          = m_item->iconName();
  b_attentionIconName = m_item->attentionIconName();
  b_overlayIconName   = m_item->overlayIconName();

  b_pixmapList          = m_item->iconPixmap();
  b_attentionPixmapList = m_item->attentionIconPixmap();
  b_overlayPixmapList   = m_item->overlayIconPixmap();
}

bool StatusNotifierItem::isValid() const { return m_item->isValid(); }
bool StatusNotifierItem::isReady() const { return m_isReady; }

QPixmap StatusNotifierItem::createPixmap(const QSize &size) {
  auto needsAttention = b_status.value() == Status::NeedsAttention;

  auto getClosestPixmap = [](const QSize                   &size,
                              const DBusTrayIconPixmapList &pixmaps) {
    const DBusTrayIconPixmap *ret = nullptr;

    for (const auto &pixmap : pixmaps) {
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

  auto getPixmapFromTheme = [](const QSize &size, const QString &iconName,
                                QPixmap *destination) {
    auto icon    = QIcon::fromTheme(iconName);
    *destination = icon.pixmap(size.width(), size.height());
  };

  auto assignPixmap = [](const QSize &size, const DBusTrayIconPixmap *source,
                          QPixmap *destination) {
    if (source != nullptr) {
      const auto image = source->createImage().scaled(
          size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      *destination = QPixmap::fromImage(image);
    }
  };

  QPixmap pixmap;

  if (needsAttention) {
    if (!b_attentionIconName.value().isEmpty()) {
      getPixmapFromTheme(size, b_attentionIconName.value(), &pixmap);
    } else {
      auto *icon = getClosestPixmap(size, b_attentionPixmapList.value());

      if (icon == nullptr) {
        // Fall back to regular icon if it fails to get an attention icon
        icon = getClosestPixmap(size, b_pixmapList.value());
      }

      assignPixmap(size, icon, &pixmap);
    }
  } else {
    if (!b_iconName.value().isEmpty()) {
      getPixmapFromTheme(size, b_iconName.value(), &pixmap);
    } else {
      auto *icon = getClosestPixmap(size, b_pixmapList.value());

      assignPixmap(size, icon, &pixmap);
    }

    QPixmap overlayPixmap;
    if (!b_overlayIconName.value().isEmpty()) {
      getPixmapFromTheme(size, b_overlayIconName.value(), &overlayPixmap);
    } else {
      auto *icon = getClosestPixmap(size, b_pixmapList.value());

      assignPixmap(size, icon, &overlayPixmap);
    }

    if (!overlayPixmap.isNull()) {
      auto painter = QPainter(&pixmap);
      painter.drawPixmap(
          QRect(0, 0, pixmap.width(), pixmap.height()), overlayPixmap);
      painter.end();
    }
  }

  return pixmap;
}

void StatusNotifierItem::refreshPixmap() {
  b_pixmapIndex = b_pixmapIndex.value() + 1;
}
} // namespace ns::systemtray
