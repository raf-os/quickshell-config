#include "kbd.h"
#include <functional>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qhash.h>
#include <qlogging.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlversion.h>
#include <libxml/xpath.h>
#include <qobject.h>
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
      target->append(QString::fromUtf8(subChild->content));
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

KKeyboardModel::KKeyboardModel(QString name, QString description,
                               QString vendor, QObject *parent)
    : QObject(parent), m_name(name), m_description(description),
      m_vendor(vendor) {}

QString KKeyboardModel::name() const { return m_name; }
QString KKeyboardModel::description() const { return m_description; }
QString KKeyboardModel::vendor() const { return m_vendor; }

KKeyboardVariant::KKeyboardVariant(QString name, QString description,
                                   QObject *parent)
    : QObject(parent), m_name(name), m_description(description) {}

QString KKeyboardVariant::name() const { return m_name; }
QString KKeyboardVariant::description() const { return m_description; }

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

  m_cachePath = path;
  emit cachePathChanged();
}

QQmlListProperty<myqmlplugin::KKeyboardLayout>
KeyboardLayoutHandler::layouts() {
  m_layoutList = m_layouts.values();
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
      xmlXPathEvalExpression((xmlChar *)"//layout/configItem", ctx);

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

  if (!m_layouts.isEmpty()) {
    emit layoutsChanged();
  }

  xmlXPathFreeObject(result);
  xmlXPathFreeContext(ctx);
  xmlFreeDoc(doc);
  xmlCleanupParser();

  return true;
}

void KeyboardLayoutHandler::traverseXmlNodes(xmlNodeSetPtr nodes) {
  xmlNodePtr cur;
  int size = (nodes) ? nodes->nodeNr : 0;

  for (int i = 0; i < size; ++i) {
    if (!nodes->nodeTab[i]) {
      break;
    }

    QString nameBuffer;
    QString descriptionBuffer;
    QString shortDescriptionBuffer;
    QStringList countryListBuffer;
    QStringList languageListBuffer;
    QList<KKeyboardVariant *> variantListBuffer;

    if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
      cur = nodes->nodeTab[i];

      for (xmlNodePtr child = cur->children; child; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
          if (xmlStrcmp(child->name, (xmlChar *)"name") == 0) {
            xmlGetChildContent(child, &nameBuffer);
          } else if (xmlStrcmp(child->name, (xmlChar *)"description") == 0) {
            xmlGetChildContent(child, &descriptionBuffer);
          } else if (xmlStrcmp(child->name, (xmlChar *)"shortDescription") ==
                     0) {
            xmlGetChildContent(child, &shortDescriptionBuffer);
          } else if (xmlStrcmp(child->name, (xmlChar *)"countryList") == 0) {
            xmlGetChildrenListContent(child, &countryListBuffer);
          } else if (xmlStrcmp(child->name, (xmlChar *)"languageList") == 0) {
            xmlGetChildrenListContent(child, &languageListBuffer);
          } else if (xmlStrcmp(child->name, (xmlChar *)"variantList") == 0) {
            // What have I created
            xmlGetNodesByName(
                child, (xmlChar *)"variants",
                [this, &variantListBuffer](xmlNodePtr c1) {
                  xmlGetNodesByName(
                      c1, (xmlChar *)"configItem",
                      [this, &variantListBuffer](xmlNodePtr c2) {
                        QString nameBuf;
                        QString descBuf;

                        for (xmlNodePtr confNode = c2->children; confNode;
                             confNode = confNode->next) {
                          if (confNode->type != XML_ELEMENT_NODE)
                            continue;
                          if (xmlStrcmp(confNode->name, (xmlChar *)"name") ==
                              0) {
                            nameBuf = QString::fromUtf8(confNode->content);
                          } else if (xmlStrcmp(confNode->name,
                                               (xmlChar *)"description") == 0) {
                            descBuf = QString::fromUtf8(confNode->content);
                          }
                        }

                        if (nameBuf != "") {
                          auto kbVar =
                              new KKeyboardVariant(nameBuf, descBuf, this);
                          variantListBuffer.append(kbVar);
                        }
                      });
                });
          }
        }
      }
    }

    if (nameBuffer != "") {
      auto layout = new KKeyboardLayout(nameBuffer, shortDescriptionBuffer,
                                        descriptionBuffer, countryListBuffer,
                                        languageListBuffer, this);
      m_layouts.insert(nameBuffer, layout);

      for (auto kbVar : variantListBuffer) {
        kbVar->setParent(layout);
        layout->addVariant(kbVar);
      }
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
} // namespace myqmlplugin
