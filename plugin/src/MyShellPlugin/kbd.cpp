#include "kbd.h"
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qlogging.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlversion.h>
#include <libxml/xpath.h>

namespace myqmlplugin {
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

QVariantMap KeyboardLayoutHandler::layouts() const { return m_layouts; }

bool KeyboardLayoutHandler::rebuildLayouts() {
  xmlInitParser();
  LIBXML_TEST_VERSION

  xmlDocPtr doc = xmlReadFile(m_evdevPath, nullptr, 0);

  if (doc == nullptr) {
    qWarning() << "myqmlplugin::KeyboardLayoutHandler: Error reading xml file.";
    return false;
  }

  xmlXPathContextPtr ctx = xmlXPathNewContext(doc);

  if (ctx == nullptr) {
    qWarning() << "myqmlplugin::KeyboardLayoutHandler: Unable to create XPath "
                  "context.";
    xmlFreeDoc(doc);
    return false;
  }

  xmlXPathObjectPtr result =
      xmlXPathEvalExpression((xmlChar *)"//layout/configItem", ctx);

  if (result == nullptr) {
    qWarning() << "myqmlplugin::KeyboardLayoutHandler: Unable to evaluate "
                  "xpath expression.";
    xmlXPathFreeContext(ctx);
    xmlFreeDoc(doc);
    return false;
  }

  m_layouts.clear();

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
    QString countryNameBuffer;
    QString descriptionBuffer;

    if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
      cur = nodes->nodeTab[i];

      for (xmlNodePtr child = cur->children; child; child = child->next) {
        if (child->type == XML_ELEMENT_NODE) {
          if (xmlStrcmp(child->name, (xmlChar *)"name") == 0) {
            if (child->children) {
              nameBuffer = QString::fromUtf8(child->children->content);
            }
          } else if (xmlStrcmp(child->name, (xmlChar *)"description") == 0) {
            if (child->children) {
              descriptionBuffer = QString::fromUtf8(child->children->content);
            }
          } else if (xmlStrcmp(child->name, (xmlChar *)"countryList") == 0) {
            for (xmlNodePtr subChild = child->children; subChild;
                 subChild = subChild->next) {
              if (subChild->type == XML_ELEMENT_NODE && subChild->children) {
                countryNameBuffer =
                    QString::fromUtf8(subChild->children->content);
                break;
              }
            }
          }
        }
      }
    }

    if (nameBuffer != "" && countryNameBuffer != "") {
      QVariantMap content;
      content.insert("countryName", countryNameBuffer);
      content.insert("description", descriptionBuffer);

      m_layouts.insert(nameBuffer, content);
    }
  }
}

void KeyboardLayoutHandler::debugPrintLayouts() {
  for (auto i = m_layouts.begin(); i != m_layouts.cend(); ++i) {
    qDebug() << "=====\n" << "NAME: " << i.key();
    auto val = i.value();

    if (val.typeId() == QMetaType::QVariantMap) {
      QVariantMap map = val.toMap();
      qDebug() << map.value("countryName", "UNSET");
      qDebug() << map.value("description", "UNSET");
    }

    qDebug() << "=====";
  }
}
} // namespace myqmlplugin
