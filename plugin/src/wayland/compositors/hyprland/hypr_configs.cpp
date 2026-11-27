#include "hypr_configs.h"

#include <optional>
#include <utility>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStringList>
#include <QStringView>
#include <qnamespace.h>

#include "hyprctl.h"
#include "keyboard_layout_serializer.h"
#include "ns_hyprland_debug.h"

namespace ns::wayland::hyprland {
namespace {
bool qsv_starts_with(QAnyStringView s, QAnyStringView prefix) {
  return s.size() >= prefix.size() && s.first(prefix.size()) == prefix;
}

bool qsv_ends_with(QAnyStringView s, QAnyStringView suffix) {
  return s.size() >= suffix.size() && s.last(suffix.size()) == suffix;
}
} // namespace

HyprConfigs::HyprConfigs(
    Hyprctl                            *hyprctl,
    utils::KeyboardLayoutXMLSerializer *keyboardLayoutXMLSerializer,
    QObject                            *parent)
    : QObject(parent), m_hyprctl(hyprctl),
      m_keyboardXmlSerializer(keyboardLayoutXMLSerializer) {
  if (!keyboardLayoutXMLSerializer->isReady()) {
    QObject::connect(
        keyboardLayoutXMLSerializer, &utils::KeyboardLayoutXMLSerializer::ready,
        this,
        [this] {
          for (auto &layout : m_keyboardLayouts) {
            auto opt = m_keyboardXmlSerializer->getConfigItem(layout.layout,
                                                              layout.variant);
            if (opt.has_value()) {
              const auto &cItem  = opt.value();
              layout.description = cItem.description.isEmpty()
                                     ? cItem.shortDescription
                                     : cItem.description;
            }
          }

          emit this->keyboardLayoutsChanged();
        },
        Qt::SingleShotConnection);
  }
}

QList<HyprKeyboardLayout> HyprConfigs::keyboardLayouts() const {
  return m_keyboardLayouts;
}

HyprKeyboardLayout HyprConfigs::activeKeyboardLayout() const {
  if (m_kbdIdx < 0 || m_kbdIdx > m_keyboardLayouts.size()) {
    return HyprKeyboardLayout{.layout = "??", .variant = "", .description = ""};
  }

  return m_keyboardLayouts.at(m_kbdIdx);
}

void HyprConfigs::setActiveLayoutIndex(qint64 index) {
  if (m_kbdIdx == index) return;

  m_kbdIdx = index;
  emit activeKeyboardLayoutChanged();
}

void HyprConfigs::queryConfigs() {
  if (!m_hyprctl) return;

  if (m_descriptionsPending) {
    m_descriptionsQueued = true;
    return;
  }

  m_descriptionsPending = true;
  m_hyprctl->call(this, "j/descriptions",
                  [this](auto success, const QByteArray &result) {
                    m_descriptionsPending = false;

                    if (!success) {
                      return;
                    }

                    this->processConfigData(result);
                  });
}

void HyprConfigs::queryDevices() {
  if (!m_hyprctl || m_devicesPending) return;

  m_devicesPending = true;
  m_hyprctl->call(this, "j/devices", [this](auto success, const auto &result) {
    m_devicesPending = false;

    if (!success) return;

    this->processDeviceData(result);
  });
}

void HyprConfigs::onActiveLayoutChanged(const QString &keyboardName,
                                        const QString &layoutName) {
  Q_UNUSED(keyboardName)

  for (auto i = 0; i < m_keyboardLayouts.size(); i++) {
    if (m_keyboardLayouts.at(i).description == layoutName) {
      setActiveLayoutIndex(i);
      break;
    }
  }

  // In case it is not found
  setActiveLayoutIndex(-1);
}

void HyprConfigs::processConfigData(const QByteArray &data) {
  QJsonParseError error;
  auto            rootDoc = QJsonDocument::fromJson(data, &error);

  if (error.error != QJsonParseError::NoError) {
    qCWarning(NS_LOG_HYPRLAND)
        << "Error parsing json config data:" << error.errorString();
    return;
  }

  if (!rootDoc.isArray()) {
    qCWarning(NS_LOG_HYPRLAND) << "Received invalid data from hyprctl "
                                  "j/descriptions. Unable to parse.";
    return;
  }

  QStringList layouts;
  QStringList variants;

  for (auto ob : rootDoc.array()) {
    if (!ob.isObject()) continue;

    auto obj = ob.toObject();

    if (!obj.contains(u"name")) continue;

    auto vname = obj.value(u"name");
    auto sv    = vname.toStringView();

    if (qsv_starts_with(sv, u8"input:kb_")) {
      auto cur = obj.value(u"current").toString();
      if (cur == "[[EMPTY]]") continue;

      auto chunks = vname.toString().split(':');
      if (chunks.size() != 2) continue;

      QStringView val = chunks.at(1);

      if (val == "kb_layout") {
        layouts = cur.split(',');
      } else if (val == "kb_variant") {
        variants = cur.split(',');
      }
    }
  }

  m_keyboardLayouts.clear();

  if (layouts.isEmpty()) {
    emit keyboardLayoutsChanged();
    return;
  }

  for (auto &layout : layouts) {
    HyprKeyboardLayout lData;

    lData.layout.swap(layout);

    if (!variants.isEmpty()) {
      lData.variant = variants.takeFirst();
    }

    if (m_keyboardXmlSerializer && m_keyboardXmlSerializer->isReady()) {
      auto opt =
          m_keyboardXmlSerializer->getConfigItem(lData.layout, lData.variant);
      if (opt.has_value()) {
        const auto &cItem = opt.value();
        lData.description = cItem.description.isEmpty() ? cItem.shortDescription
                                                        : cItem.description;
      }
    }

    m_keyboardLayouts.append(std::move(lData));
  }

  if (m_keyboardXmlSerializer && m_keyboardXmlSerializer->isReady())
    emit keyboardLayoutsChanged();
}

void HyprConfigs::processDeviceData(const QByteArray &data) {
  if (data.isEmpty()) return;

  QJsonParseError error;
  auto            rootDoc = QJsonDocument::fromJson(data, &error);

  if (error.error != QJsonParseError::NoError) {
    qCWarning(NS_LOG_HYPRLAND)
        << "Error parsing j/devices:" << error.errorString();
    return;
  }

  if (!rootDoc.isObject()) {
    qCWarning(NS_LOG_HYPRLAND)
        << "Received invalid data from j/devices. Unable to parse.";
    return;
  }

  auto arr = rootDoc.object().value("keyboards");
  if (!arr.isArray()) return;
  auto keyboardsArray = arr.toArray();

  for (auto k : keyboardsArray) {
    if (!k.isObject()) continue;

    auto       keyboard = k.toObject();
    const auto isMain   = keyboard.value("main").toBool(false);

    if (!isMain) continue;

    const auto layoutIndex = keyboard.value("active_layout_index").toInteger(0);

    setActiveLayoutIndex(layoutIndex);
    break;
  }
}

} // namespace ns::wayland::hyprland
