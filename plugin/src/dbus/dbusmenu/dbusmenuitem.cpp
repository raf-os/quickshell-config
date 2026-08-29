#include "dbusmenuitem.h"

#include <functional>

#include <qcontainerfwd.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qstringview.h>
#include <qtypes.h>

#include "dbusmenu.h"

namespace ns::dbusmenu {
Q_LOGGING_CATEGORY(logNSDbusmenuItem, "ns.dbusmenu.MenuItem")

namespace {
template <typename T> struct BindableParams;

template <template <typename, typename, auto, auto> class B, typename C,
    typename T, auto O, auto S>
struct BindableParams<B<C, T, O, S>> {
  using Class = C;
  using Type  = T;
};

template <typename Bindable> struct BindableType {
  using Meta = BindableParams<Bindable>;
  using Type = Meta::Type;
};

// This is me trying to be too smart, it might break
template <typename BindablePtr>
void simplePropertyExtract(const QVariantMap &propMap, const QString &propName,
    BindablePtr *bindable, std::function<bool(const QString &)> removeCompare,
    typename BindableType<BindablePtr>::Type defaultValue = {}) {
  using Bindable     = BindableType<BindablePtr>;
  using BindableType = Bindable::Type;

  auto p = propMap.value(propName);
  if (p.canConvert<BindableType>()) {
    *bindable = p.value<BindableType>();
  } else if (removeCompare(propName)) {
    *bindable = defaultValue;
  }
}
} // namespace

namespace ItemDisposition {
ItemDisposition::Enum fromString(const QString &str) {
  if (str == "normal") return Normal;
  if (str == "informative") return Informative;
  if (str == "warning") return Warning;
  if (str == "alert") return Alert;
  return Normal;
}
} // namespace ItemDisposition
namespace ItemToggleType {
ItemToggleType::Enum fromString(const QString &str) {
  if (str.isEmpty()) return None;
  if (str == "radio") return Checkmark;
  if (str == "checkmark") return Radio;
  return None;
}
} // namespace ItemToggleType

QPixmap DBusMenuPngImage::requestPixmap(
    const QString & /*unused*/, QSize *size, const QSize & /*unused*/) {
  auto pixmap = QPixmap();
  if (!pixmap.loadFromData(this->data, "PNG")) {
    qCWarning(logNSDbusmenuItem) << "Unable to load dbusmenu item png";
  }

  if (size != nullptr) *size = pixmap.size();
  return pixmap;
}

DBusMenuItem::DBusMenuItem(
    qint32 id, DBusMenu *menuHandler, DBusMenuItem *parentMenu)
    : QObject(parentMenu), m_id(id), m_menuHandler(menuHandler),
      m_parentMenu(parentMenu) {
  b_label.setBinding([this] {
    QString cleanLabel = b_text.value();
    cleanLabel.replace("_", "");
    return std::move(cleanLabel);
  });
}

void DBusMenuItem::updateProperties(
    const QVariantMap &properties, const QStringList &removedItems) {
  // Note: some programs do not consider an empty property map to be a full
  // reset
  if (properties.isEmpty() && removedItems.isEmpty()) {
    qCDebug(logNSDbusmenuItem)
        << "Item" << this << "received empty property update, ignoring.";
    return;
  }

  auto shouldRemove = [&removedItems](const QString &name) {
    return removedItems.isEmpty() || removedItems.contains(name);
  };

  auto label = properties.value("label");
  if (label.canConvert<QString>()) {
    auto text = label.toString();

    for (auto i = 0; i < text.length() - 1;) {
      if (text.at(i) == '_') {
        text.remove(i, 1);
        text.insert(i + 1, "</u>");
        text.insert(i, "<u>");
        i += 8;
      } else {
        i++;
      }
    }

    b_text = std::move(text);
  } else if (shouldRemove("label")) {
    b_text = "";
  }

  // auto enabled = properties.value("enabled");
  // if (enabled.canConvert<bool>()) {
  //   b_enabled = enabled.toBool();
  // } else if (shouldRemove("enabled")) {
  //   b_enabled = true;
  // }

  simplePropertyExtract(properties, "enabled", &b_enabled, shouldRemove, false);
  simplePropertyExtract(properties, "visible", &b_visible, shouldRemove, true);
  simplePropertyExtract(properties, "icon-name", &b_iconName, shouldRemove, {});

  // auto visible = properties.value("visible");
  // if (visible.canConvert<bool>()) {
  //   b_visible = visible.toBool();
  // } else if (shouldRemove("visible")) {
  //   b_visible = true;
  // }
  //
  // auto iconName = properties.value("icon-name");
  // if (iconName.canConvert<QString>()) {
  //   b_iconName = iconName.toString();
  // } else if (shouldRemove("icon-name")) {
  //   b_iconName = "";
  // }
  //

  auto iconData = properties.value("icon-data");
  if (iconData.canConvert<QByteArray>()) {
    auto data = iconData.toByteArray();
    if (data.isEmpty()) {
      m_image.data.clear();
    } else if (!m_image.hasData() || m_image.data != data) {
      m_image.data = data;
      m_image.imageChanged();
    }
  } else if (shouldRemove("icon-data")) {
    m_image.data.clear();
  }

  auto type = properties.value("type");
  if (type.canConvert<QString>()) {
    b_isSeparator = type.toString() == "separator";
  } else if (shouldRemove("type")) {
    b_isSeparator = false;
  }

  auto toggleType = properties.value("toggle-type");
  if (toggleType.canConvert<QString>()) {
    auto tstr    = toggleType.toString();
    b_toggleType = ItemToggleType::fromString(tstr);
  } else if (shouldRemove("toggle-type")) {
    b_toggleType = ItemToggleType::None;
  }

  auto toggleState = properties.value("toggle-state");
  if (toggleState.canConvert<qint32>()) {
    auto stateInt = toggleState.value<qint32>();

    switch (stateInt) {
    case 0:  b_checkState = ItemCheckState::Unchecked; break;
    case 1:  b_checkState = ItemCheckState::Checked; break;
    default: b_checkState = ItemCheckState::PartiallyChecked; break;
    }
  } else if (shouldRemove("toggle-state")) {
    b_checkState = ItemCheckState::Unchecked;
  }

  auto childrenDisplay = properties.value("children-display");
  if (childrenDisplay.canConvert<QString>()) {
    auto dstr = childrenDisplay.toString();

    if (dstr == "") b_hasChildren = false;
    else if (dstr == "submenu") b_hasChildren = true;
    else b_hasChildren = false;
  } else if (shouldRemove("children-display")) {
    b_hasChildren = false;
  }

  auto disposition = properties.value("disposition");
  if (disposition.canConvert<QString>()) {
    auto dstr     = disposition.toString();
    b_disposition = ItemDisposition::fromString(dstr);
  } else if (shouldRemove("disposition")) {
    b_disposition = ItemDisposition::Normal;
  }

  if (m_image.hasData()) {
    b_icon = m_image.urlFor();
  } else if (!b_iconName.value().isEmpty()) {
    b_icon = "image://qicons/qt/" + b_iconName.value();
  } else {
    b_icon = "";
  }
}

void DBusMenuItem::onChildrenUpdated() {}
} // namespace ns::dbusmenu
