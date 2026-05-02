#include "kbd.h"
#include <algorithm>
#include <functional>
#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlversion.h>
#include <libxml/xpath.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmllist.h>

namespace myqmlplugin {
void xmlGetChildContent(xmlNodePtr child, QString *target) {
  if (child->children) {
    *target = QString::fromUtf8(child->children->content);
  }
}

void xmlGetChildrenListContent(xmlNodePtr child, QStringList *target) {
  for (xmlNodePtr subChild = child->children; subChild;
       subChild = subChild->next) {
    if (subChild->type == XML_ELEMENT_NODE && subChild->children) {
      target->append(QString::fromUtf8(subChild->children->content));
    }
  }
}

void xmlGetNodesByName(xmlNodePtr child, xmlChar *nodeName,
                       std::function<void(xmlNodePtr)> callback) {
  for (xmlNodePtr subChild = child->children; subChild;
       subChild = subChild->next) {
    if (subChild->type == XML_ELEMENT_NODE && subChild->children &&
        xmlStrcmp(subChild->name, nodeName) == 0) {
      callback(subChild);
    }
  }
};

bool xmlConditionalAssign(xmlNodePtr node, const char *name, QString *target) {
  if (xmlStrcmp(node->name, (xmlChar *)name) == 0) {
    xmlGetChildContent(node, target);
    return true;
  } else
    return false;
}

bool xmlConditionalAssign(xmlNodePtr node, const char *name,
                          QStringList *target) {
  if (xmlStrcmp(node->name, (xmlChar *)name) == 0) {
    xmlGetChildrenListContent(node, target);
    return true;
  } else
    return false;
}

void xmlProcessConfigItem(KKeyboardConfigItemData &confStruct,
                          xmlNodePtr confNode) {
  QString nameBuffer;
  QString descriptionBuffer;
  QString shortDescriptionBuffer;
  QStringList countryListBuffer;
  QStringList languageListBuffer;

  // TODO: Maybe try to improve this unholy creation

  for (xmlNodePtr child = confNode->children; child; child = child->next) {
    if (child->type == XML_ELEMENT_NODE) {
      if (xmlConditionalAssign(child, "name", &nameBuffer))
        continue;
      if (xmlConditionalAssign(child, "description", &descriptionBuffer))
        continue;
      if (xmlConditionalAssign(child, "shortDescription",
                               &shortDescriptionBuffer))
        continue;
      if (xmlConditionalAssign(child, "countryList", &countryListBuffer))
        continue;
      if (xmlConditionalAssign(child, "languageList", &languageListBuffer))
        continue;
    }
  }

  confStruct.name = nameBuffer;
  confStruct.description = descriptionBuffer;
  confStruct.shortDescription = shortDescriptionBuffer;
  confStruct.countryList = countryListBuffer;
  confStruct.languageList = languageListBuffer;
}

KKeyboardModel::KKeyboardModel(QString name, QString description,
                               QString vendor, QObject *parent)
    : QObject(parent), m_name(name), m_description(description),
      m_vendor(vendor) {}

QString KKeyboardModel::name() const { return m_name; }
QString KKeyboardModel::description() const { return m_description; }
QString KKeyboardModel::vendor() const { return m_vendor; }

KKeyboardVariant::KKeyboardVariant(QString name, QString description,
                                   QString shortDescription,
                                   QStringList languageList, QObject *parent)
    : QObject(parent), m_name(name), m_description(description),
      m_shortDescription(shortDescription), m_languageList(languageList) {}

QString KKeyboardVariant::name() const { return m_name; }
QString KKeyboardVariant::description() const { return m_description; }
QString KKeyboardVariant::shortDescription() const {
  return m_shortDescription;
}
QStringList KKeyboardVariant::languageList() const { return m_languageList; }

KKeyboardLayout::KKeyboardLayout(QString name, QString shortDescription,
                                 QString description, QStringList countryList,
                                 QStringList languageList, QObject *parent)
    : QObject(parent), m_name(name), m_shortDescription(shortDescription),
      m_description(description), m_countryList(countryList),
      m_languageList(languageList) {}

QString KKeyboardLayout::name() const { return m_name; }
QString KKeyboardLayout::shortDescription() const { return m_shortDescription; }
QString KKeyboardLayout::description() const { return m_description; }
QStringList KKeyboardLayout::countryList() const { return m_countryList; }
QStringList KKeyboardLayout::languageList() const { return m_languageList; }

QList<KKeyboardVariant *> KKeyboardLayout::variantList() const {
  return m_variants;
}

QQmlListProperty<myqmlplugin::KKeyboardVariant> KKeyboardLayout::variants() {
  return QQmlListProperty<KKeyboardVariant>(this, &m_variants);
}

void KKeyboardLayout::addVariant(KKeyboardVariant *variant) {
  variant->setParent(this);
  m_variants.append(variant);
}

KKeyboardVariant *KKeyboardLayout::getVariantByName(const QString &name) {
  KKeyboardVariant *variantBuffer = nullptr;
  for (auto v : m_variants) {
    if (v->name() == name) {
      variantBuffer = v;
      break;
    }
  }
  return variantBuffer;
}

KeyboardLayoutHandler::KeyboardLayoutHandler(QObject *parent)
    : QObject(parent) {
  QFile file(QString::fromUtf8(m_evdevPath));

  if (!file.exists()) {
    qWarning() << "myqmlplugin::KeyboardLayoutHandler: Could not find "
                  "evdev.xml file. Aborting.";
    deleteLater();
    return;
  }

  rebuildLayouts();
}

QString KeyboardLayoutHandler::cachePath() const { return m_cachePath; }

void KeyboardLayoutHandler::setCachePath(const QString &path) {
  if (m_cachePath == path) {
    return;
  }

  QDir dir(path);

  if (!dir.exists()) {
    if (!dir.mkpath(".")) {
      qWarning() << "myqmlplugin::KeyboardLayoutHandler:setCachePath: Error "
                    "creating specified cache path.";
      return;
    }
  }

  m_cachePath = path;
  saveToJsonCache();
  emit cachePathChanged();
}

QQmlListProperty<myqmlplugin::KKeyboardLayout>
KeyboardLayoutHandler::layouts() {
  return QQmlListProperty<KKeyboardLayout>(this, &m_layoutList);
}

QQmlListProperty<myqmlplugin::KKeyboardModel> KeyboardLayoutHandler::models() {
  m_modelList = m_models.values();
  return QQmlListProperty<KKeyboardModel>(this, &m_modelList);
}

bool KeyboardLayoutHandler::rebuildLayouts() {
  xmlInitParser();
  LIBXML_TEST_VERSION

  xmlDocPtr doc = xmlReadFile(m_evdevPath, nullptr, 0);

  if (doc == nullptr) {
    qWarning() << "myqmlplugin::KeyboardLayoutHandler: Error reading xml file.";
    xmlCleanupParser();
    return false;
  }

  xmlXPathContextPtr ctx = xmlXPathNewContext(doc);

  if (ctx == nullptr) {
    qWarning() << "myqmlplugin::KeyboardLayoutHandler: Unable to create XPath "
                  "context.";
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return false;
  }

  xmlXPathObjectPtr result =
      xmlXPathEvalExpression((xmlChar *)"//layoutList/layout", ctx);

  if (result == nullptr) {
    qWarning() << "myqmlplugin::KeyboardLayoutHandler: Unable to evaluate "
                  "xpath expression.";
    xmlXPathFreeContext(ctx);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return false;
  }

  for (auto layout : m_layouts) {
    layout->deleteLater();
  }

  for (auto model : m_models) {
    model->deleteLater();
  }

  m_layouts.clear();
  m_models.clear();

  traverseXmlNodes(result->nodesetval);

  m_layoutList = m_layouts.values();
  std::sort(m_layoutList.begin(), m_layoutList.end(),
            [this](KKeyboardLayout *a, KKeyboardLayout *b) {
              return a->name().localeAwareCompare(b->name()) < 0;
            });

  if (!m_layouts.isEmpty()) {
    emit layoutsChanged();
  }

  xmlXPathFreeObject(result);
  xmlXPathFreeContext(ctx);
  xmlFreeDoc(doc);
  xmlCleanupParser();

  saveToJsonCache();

  return true;
}

void KeyboardLayoutHandler::traverseXmlNodes(xmlNodeSetPtr nodes) {
  xmlNodePtr cur;
  int size = (nodes) ? nodes->nodeNr : 0;

  for (int i = 0; i < size; ++i) {
    if (!nodes->nodeTab[i]) {
      break;
    }

    KKeyboardConfigItemData configItem;
    QList<KKeyboardVariant *> variantListBuffer;

    if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
      cur = nodes->nodeTab[i];

      for (xmlNodePtr section = cur->children; section;
           section = section->next) {
        if (section->type == XML_ELEMENT_NODE) {
          if (xmlStrcmp(section->name, (xmlChar *)"configItem") ==
              0) { // CONFIGITEM
            xmlProcessConfigItem(configItem, section);
          } // CONFIGITEM
          else if (xmlStrcmp(section->name, (xmlChar *)"variantList") ==
                   0) { // VARIANTLIST
            for (xmlNodePtr child = cur->children; child; child = child->next) {
              xmlGetNodesByName(
                  child, (xmlChar *)"variant",
                  [this, &variantListBuffer](xmlNodePtr c1) {
                    xmlGetNodesByName(
                        c1, (xmlChar *)"configItem",
                        [this, &variantListBuffer](xmlNodePtr c2) {
                          KKeyboardConfigItemData variantConfigItem;

                          xmlProcessConfigItem(variantConfigItem, c2);

                          if (variantConfigItem.name != "") {
                            auto kbVar = new KKeyboardVariant(
                                variantConfigItem.name,
                                variantConfigItem.description,
                                variantConfigItem.shortDescription,
                                variantConfigItem.languageList, this);
                            variantListBuffer.append(kbVar);
                          }
                        });
                  });
            }
          } // VARIANTLIST
        }
      }
    }

    if (configItem.name != "") {
      auto layout = new KKeyboardLayout(
          configItem.name, configItem.shortDescription, configItem.description,
          configItem.countryList, configItem.languageList, this);
      m_layouts.insert(configItem.name, layout);

      for (auto kbVar : variantListBuffer) {
        kbVar->setParent(layout);
        layout->addVariant(kbVar);
      }

      emit layout->variantsChanged();
    }
  }
}

KKeyboardLayout *KeyboardLayoutHandler::findLayoutByName(const QString &name) {
  QHash<QString, KKeyboardLayout *>::const_iterator it = m_layouts.find(name);
  if (it != m_layouts.end()) {
    return it.value();
  }
  return nullptr;
}

void KeyboardLayoutHandler::debugPrintLayouts() {}

void KeyboardLayoutHandler::saveToJsonCache() {
  if (m_cachePath == "") {
    return;
  }

  QFile file(m_cachePath + "/kbd_cache.json");
  QFileInfo fileInfo(file);

  if (fileInfo.exists()) {
    auto currentDate = QDateTime::currentDateTime();
    auto fileDate = fileInfo.lastModified().addSecs(60 * 60 * 4); // 4 hours

    if (fileDate > currentDate) {
      return;
    }
  }

  QJsonObject kbdJson;
  QJsonArray kbLayouts;

  for (auto it = m_layouts.cbegin(); it != m_layouts.cend(); ++it) {
    auto l = it.value();
    QJsonObject kl;
    kl["name"] = l->name();
    kl["description"] = l->description();
    kl["shortDescription"] = l->shortDescription();
    kl["countryList"] = l->countryList().join(",");
    kl["languageList"] = l->languageList().join(",");
    if (auto variants = l->variantList(); variants.size() > 0) {
      QJsonArray varList;
      for (auto const &variant : variants) {
        QJsonObject v;
        v["name"] = variant->name();
        v["description"] = variant->description();
        varList.append(v);
      }
      kl["variants"] = varList;
    }
    kbLayouts.append(kl);
  }

  kbdJson["layouts"] = kbLayouts;

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "myqmlplugin::KeyboardLayoutHandler::saveToJsonCache: Error "
                  "opening keyboard cache to write - aborting.";
    return;
  }

  QTextStream out(&file);

  out << QJsonDocument(kbdJson).toJson(QJsonDocument::Compact);

  file.close();
}
} // namespace myqmlplugin
