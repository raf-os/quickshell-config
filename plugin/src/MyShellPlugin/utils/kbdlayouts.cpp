#include "kbdlayouts.h"

#include <algorithm>
#include <optional>
#include <utility>

#include <qcontainerfwd.h>
#include <qdir.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtypes.h>
#include <qxmlstream.h>

namespace ns::utils {
Q_LOGGING_CATEGORY(logNSUtilsKeyboard,
                   "nightshell.utils.keyboards")

namespace {
void xmlReadList(QXmlStreamReader &xml,
                 QStringList      &target) {
  while (xml.readNextStartElement()) {
    auto content = xml.readElementText();
    if (!xml.hasError() && !content.isEmpty()) {
      if (!target.contains(content)) {
        target.append(content);
      }
    }
  }
};
} // namespace

CKeyboardVariant::CKeyboardVariant(SKeyboardVariant payload,
                                   QObject         *parent)
    : QObject(parent),
      data(std::move(payload)) {}

QString CKeyboardVariant::name() const { return this->data.name; }
QString CKeyboardVariant::shortDescription() const {
  return this->data.shortDescription;
}
QString CKeyboardVariant::description() const { return this->data.description; }
QStringList CKeyboardVariant::languageList() const {
  return this->data.languageList;
}

CKeyboardLayout::CKeyboardLayout(SKeyboardLayout payload,
                                 QObject        *parent)
    : QObject(parent),
      data(std::move(payload)) {
  if (!this->data.variants.isEmpty()) {
    this->setVariants(this->data.variants);
  }
}

QString CKeyboardLayout::name() const { return this->data.name; }
QString CKeyboardLayout::shortDescription() const {
  return this->data.shortDescription;
}
QString CKeyboardLayout::description() const { return this->data.description; }
QStringList CKeyboardLayout::countryList() const {
  return this->data.countryList;
}
QStringList CKeyboardLayout::languageList() const {
  return this->data.languageList;
}

QQmlListProperty<CKeyboardVariant> CKeyboardLayout::variants() {
  return QQmlListProperty<CKeyboardVariant>(
      this,
      &m_variantList,
      [](QQmlListProperty<CKeyboardVariant> *property) {
        auto *list = static_cast<QList<CKeyboardVariant *> *>(property->data);
        return list->count();
      },
      [](QQmlListProperty<CKeyboardVariant> *property, qsizetype index) {
        auto *list = static_cast<QList<CKeyboardVariant *> *>(property->data);
        return list->at(index);
      });
}
QList<CKeyboardVariant *> CKeyboardLayout::variantList() const {
  return m_variantList;
}
// Warning: list will be consumed
void CKeyboardLayout::setVariants(QList<SKeyboardVariant> &variants) {
  auto old = m_variants;

  for (auto &variant : variants) {
    const auto hasOld = old.constFind(variant.name);
    if (hasOld != old.constEnd()) {
      old.remove(hasOld.key());
      continue;
    }

    auto v = new CKeyboardVariant(std::move(variant), this);
    m_variants.insert(v->name(), v);
  }

  variants.clear();

  auto vlist = m_variants.values();
  std::sort(m_variantList.begin(),
            m_variantList.end(),
            [](CKeyboardVariant *a, CKeyboardVariant *b) {
              return a->name().localeAwareCompare(b->name()) < 0;
            });

  if (vlist != m_variantList) {
    m_variantList = vlist;
    emit variantsChanged();
  }

  for (auto it = old.begin(); it != old.end(); ++it) {
    auto v = it.value();
    if (v) v->deleteLater();
  }
}

KeyboardLayouts::KeyboardLayouts(QObject *parent) : QObject(parent) {
  this->buildDatabase();
}

QQmlListProperty<CKeyboardLayout> KeyboardLayouts::layouts() {
  return QQmlListProperty<CKeyboardLayout>(
      this,
      &m_sortedLayoutList,
      [](QQmlListProperty<CKeyboardLayout> *property) {
        auto *list = static_cast<QList<CKeyboardLayout *> *>(property->data);
        return list->count();
      },
      [](QQmlListProperty<CKeyboardLayout> *property, qsizetype index) {
        auto *list = static_cast<QList<CKeyboardLayout *> *>(property->data);
        return list->at(index);
      });
}
QList<CKeyboardLayout *> KeyboardLayouts::getSortedLayoutList() const {
  return m_sortedLayoutList;
}

void KeyboardLayouts::buildDatabase() {
  QFile file(m_evdevPath);
  if (!file.exists()) {
    qCWarning(logNSUtilsKeyboard) << "evdev xml file was not found!";
    return;
  }

  for (auto it = m_layouts.begin(); it != m_layouts.end(); ++it) {
    if (it.value()) it.value()->deleteLater();
  }
  m_layouts.clear();
  m_sortedLayoutList.clear();

  QXmlStreamReader xml(&file);

  while (xml.readNextStartElement()) {
    if (xml.name() == "xkbConfigRegistry") {
      readXmlBody(xml);
    } else {
      xml.skipCurrentElement();
    }
  }

  m_sortedLayoutList = m_layouts.values();
  std::sort(m_sortedLayoutList.begin(),
            m_sortedLayoutList.end(),
            [](CKeyboardLayout *a, CKeyboardLayout *b) {
              return a->name().localeAwareCompare(b->name()) < 0;
            });
}

void KeyboardLayouts::readXmlBody(QXmlStreamReader &xml) {
  while (xml.readNextStartElement()) {
    if (xml.name() == "layoutList") {
      readXmlLayoutList(xml);
    } else {
      xml.skipCurrentElement();
    }
  }
}

void KeyboardLayouts::readXmlLayoutList(QXmlStreamReader &xml) {
  while (xml.readNextStartElement()) {
    if (xml.name() == "layout") {
      while (xml.readNextStartElement()) {
        SKeyboardLayout cLayout;

        if (xml.name() == "configItem") {
          readXmlConfigItem(xml, cLayout);
        } else if (xml.name() == "variantList") {
          readXmlVariantList(xml, cLayout);
        } else {
          xml.skipCurrentElement();
        }

        if (!cLayout.name.isEmpty() &&
            m_layouts.constFind(cLayout.name) == m_layouts.constEnd()) {
          auto l = new CKeyboardLayout(cLayout, this);
          m_layouts.insert(l->name(), l);
        }

        break;
      }
    } else {
      xml.skipCurrentElement();
    }
  }
}

void KeyboardLayouts::readXmlConfigItem(QXmlStreamReader &xml,
                                        SKeyboardLayout  &layout) {
  while (xml.readNextStartElement()) {
    if (xml.name() == "name") layout.name = xml.readElementText();
    else if (xml.name() == "shortDescription")
      layout.shortDescription = xml.readElementText();
    else if (xml.name() == "description")
      layout.description = xml.readElementText();
    else if (xml.name() == "countryList") xmlReadList(xml, layout.countryList);
    else if (xml.name() == "languageList")
      xmlReadList(xml, layout.languageList);
    else {
      xml.skipCurrentElement();
    }
  }
}

void KeyboardLayouts::readXmlVariantList(QXmlStreamReader &xml,
                                         SKeyboardLayout  &layout) {
  while (xml.readNextStartElement()) {
    if (xml.name() == "variant") {
      while (xml.readNextStartElement()) {
        if (xml.name() == "configItem") {
          SKeyboardVariant kLayout;

          while (xml.readNextStartElement()) {
            if (xml.name() == "name") kLayout.name = xml.readElementText();
            else if (xml.name() == "description")
              kLayout.description = xml.readElementText();
            else if (xml.name() == "shortDescription")
              kLayout.shortDescription = xml.readElementText();
            else if (xml.name() == "languageList")
              xmlReadList(xml, kLayout.languageList);
            else xml.skipCurrentElement();
          }

          if (!kLayout.name.isEmpty()) {
            layout.variants.append(std::move(kLayout));
          }
          break;
        } else {
          xml.skipCurrentElement();
        }
      }
    } else {
      xml.skipCurrentElement();
    }
  }
}

std::optional<SLayoutMetadata>
KeyboardLayouts::fetchLayoutMetadata(QStringView layout,
                                     QStringView variant) {
  auto it = m_layouts.constFind(layout);
  if (it == m_layouts.constEnd()) {
    return std::nullopt;
  }

  auto foundLayout = it.value();

  if (variant.isEmpty()) {
    return SLayoutMetadata{
        foundLayout->name(), QString(), foundLayout->description()};
  }

  auto vit = std::ranges::find_if(foundLayout->variantList().begin(),
                                  foundLayout->variantList().end(),
                                  [variant](CKeyboardVariant *itPred) {
                                    return itPred->name() == variant;
                                  });
  if (vit == foundLayout->variantList().end()) {
    return std::nullopt;
  }

  const auto foundVariant = *vit;
  return SLayoutMetadata{
      foundLayout->name(), foundVariant->name(), foundVariant->description()};
}
} // namespace ns::utils
