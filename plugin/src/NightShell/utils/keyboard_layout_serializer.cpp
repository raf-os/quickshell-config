#include "keyboard_layout_serializer.h"

#include <optional>
#include <utility>

#include <qbuffer.h>
#include <qfile.h>
#include <qhashfunctions.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qxmlstream.h>

#include "ns_utils_debug.h"

namespace ns::utils {
namespace {
const char *DEFAULT_PATH = "/usr/share/X11/xkb/rules/evdev.xml";
}

KeyboardLayoutXMLSerializer::KeyboardLayoutXMLSerializer(
    const char *xmlPath, QObject *parent)
    : QObject(parent) {
  QString path{xmlPath};
  if (path.isEmpty()) path = DEFAULT_PATH;

  serialize(path);
}

bool KeyboardLayoutXMLSerializer::serialize(const QString &path) {
  QFile file{path};
  if (!file.exists()) {
    qCWarning(NS_LOG_UTILS)
        << "KeyboardLayoutXMLSerializer: file does not exist:" << path;
    return false;
  }

  if (!file.open(QIODevice::ReadOnly)) {
    qCWarning(NS_LOG_UTILS)
        << "KeyboardLayoutXMLSerializer: unable to open file for reading:"
        << path;
    return false;
  }

  m_layouts.clear();

  QXmlStreamReader xml{&file};

  while (xml.readNextStartElement()) {
    if (xml.name() == "xkbConfigRegistry") {
      readEvdevBody(xml);
    } else {
      xml.skipCurrentElement();
    }
  }

  if (xml.hasError()) {
    qCWarning(NS_LOG_UTILS)
        << "KeyboardLayoutXMLSerializer: error parsing evdev xml:"
        << xml.errorString();
    m_layouts.clear();
    return false;
  }

  emit ready();
  return true;
}

void KeyboardLayoutXMLSerializer::readEvdevBody(QXmlStreamReader &xml) {
  while (xml.readNextStartElement()) {
    if (xml.name() == "layoutList") {
      while (xml.readNextStartElement()) {
        if (xml.name() == "layout") {
          readLayoutBody(xml);
        } else {
          xml.skipCurrentElement();
        }
      }
    } else {
      xml.skipCurrentElement();
    }
  }
}

void KeyboardLayoutXMLSerializer::readLayoutBody(QXmlStreamReader &xml) {
  KLayout layout;
  while (xml.readNextStartElement()) {
    if (xml.name() == "configItem") {
      auto item = readConfigItem(xml);
      if (!item.name.isEmpty()) {
        layout.item = std::move(item);
      }
    } else if (xml.name() == "variantList") {
      while (xml.readNextStartElement()) {
        if (xml.name() == "variant") {
          while (xml.readNextStartElement()) {
            if (xml.name() == "configItem") {
              auto variant = readConfigItem(xml);
              if (!variant.name.isEmpty()) {
                layout.variants.insert(variant.name, std::move(variant));
              }
            } else xml.skipCurrentElement();
          }
        } else xml.skipCurrentElement();
      }
    } else xml.skipCurrentElement();
  }
  if (!layout.item.name.isEmpty()) {
    m_layouts.insert(layout.item.name, std::move(layout));
  }
}

KConfigItem KeyboardLayoutXMLSerializer::readConfigItem(QXmlStreamReader &xml) {
  KConfigItem cItem;
  while (xml.readNextStartElement()) {
    if (xml.name() == "name") {
      cItem.name = xml.readElementText();
    } else if (xml.name() == "shortDescription") {
      cItem.shortDescription = xml.readElementText();
    } else if (xml.name() == "description") {
      cItem.description = xml.readElementText();
    } else {
      xml.skipCurrentElement();
    }
  }
  return cItem;
}

KConfigTable KeyboardLayoutXMLSerializer::getLayoutTable() const {
  return m_layouts;
}

const KLayout *KeyboardLayoutXMLSerializer::getLayoutByName(
    const QString &name) const {
  auto it = m_layouts.constFind(name);
  if (it == m_layouts.constEnd()) return nullptr;

  return &it.value();
}

std::optional<KLayoutMetadata> KeyboardLayoutXMLSerializer::getConfigItem(
    const QString &name, const QString &variant) const {
  auto it = m_layouts.constFind(name);
  if (it == m_layouts.constEnd()) return {};

  const auto      main = it.value();
  KLayoutMetadata meta{.name = main.item.name,
      .description           = main.item.description,
      .shortDescription      = main.item.shortDescription};

  if (variant.isEmpty()) {
    return meta;
  }

  const auto variantsMap = it.value().variants;
  if (auto itt = variantsMap.constFind(variant); itt != variantsMap.constEnd())
  {
    auto variant = itt.value();
    meta.variant = variant.name;
    if (!variant.description.isEmpty()) meta.description = variant.description;
    if (!variant.shortDescription.isEmpty())
      meta.shortDescription = variant.shortDescription;
    return meta;
  }

  return {};
}
} // namespace ns::utils
