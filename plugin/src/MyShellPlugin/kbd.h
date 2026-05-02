#pragma once

#include <libxml/parser.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qlist.h>
#include <qmap.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtmetamacros.h>

namespace myqmlplugin {
struct KKeyboardConfigItemData {
  QString name{};
  QString shortDescription{};
  QString description{};
  QStringList languageList{};
  QStringList countryList{};
};

bool xmlConditionalAssign(xmlNodePtr node, char *name, QString *target);
bool xmlConditionalAssign(xmlNodePtr node, char *name, QStringList *target);

class KKeyboardModel : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)
  Q_PROPERTY(QString vendor READ name CONSTANT)

public:
  explicit KKeyboardModel(QString name, QString description, QString vendor,
                          QObject *parent = nullptr);

  [[nodiscard]] QString name() const;
  [[nodiscard]] QString description() const;
  [[nodiscard]] QString vendor() const;

private:
  QString m_name;
  QString m_description;
  QString m_vendor;
};

class KKeyboardVariant : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)
  Q_PROPERTY(QString shortDescription READ shortDescription CONSTANT)
  Q_PROPERTY(QStringList languageList READ languageList CONSTANT)

public:
  explicit KKeyboardVariant(QString name, QString description,
                            QString shortDescription, QStringList languageList,
                            QObject *parent = nullptr);

  [[nodiscard]] QString name() const;
  [[nodiscard]] QString description() const;
  [[nodiscard]] QString shortDescription() const;
  [[nodiscard]] QStringList languageList() const;

private:
  QString m_name;
  QString m_description;
  QString m_shortDescription;
  QStringList m_languageList;
};

class KKeyboardLayout : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString shortDescription READ shortDescription CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)
  Q_PROPERTY(QStringList countryList READ countryList CONSTANT)
  Q_PROPERTY(QStringList languageList READ languageList CONSTANT)
  Q_PROPERTY(QQmlListProperty<myqmlplugin::KKeyboardVariant> variants READ
                 variants NOTIFY variantsChanged)

public:
  explicit KKeyboardLayout(QString name, QString shortDescription,
                           QString description, QStringList countryList,
                           QStringList languageList, QObject *parent = nullptr);

  [[nodiscard]] QString name() const;
  [[nodiscard]] QString shortDescription() const;
  [[nodiscard]] QString description() const;
  [[nodiscard]] QStringList countryList() const;
  [[nodiscard]] QStringList languageList() const;
  [[nodiscard]] QQmlListProperty<myqmlplugin::KKeyboardVariant> variants();
  void addVariant(KKeyboardVariant *variant);

  [[nodiscard]] QList<KKeyboardVariant *> variantList() const;

  [[nodiscard]] KKeyboardVariant *getVariantByName(const QString &name);

signals:
  void variantsChanged();

private:
  QString m_name;
  QString m_shortDescription;
  QString m_description;
  QStringList m_countryList;
  QStringList m_languageList;
  QList<KKeyboardVariant *> m_variants;
};

class KeyboardLayoutHandler : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString cachePath READ cachePath WRITE setCachePath NOTIFY
                 cachePathChanged)
  Q_PROPERTY(QQmlListProperty<myqmlplugin::KKeyboardModel> models READ models
                 NOTIFY modelsChanged)
  Q_PROPERTY(QQmlListProperty<myqmlplugin::KKeyboardLayout> layouts READ layouts
                 NOTIFY layoutsChanged)

public:
  explicit KeyboardLayoutHandler(QObject *parent = nullptr);

  [[nodiscard]] QQmlListProperty<KKeyboardModel> models();
  [[nodiscard]] QQmlListProperty<KKeyboardLayout> layouts();

  [[nodiscard]] QString cachePath() const;
  void setCachePath(const QString &path);

  [[nodiscard]] KKeyboardLayout *findLayoutByName(const QString &name);

  Q_INVOKABLE void debugPrintLayouts();

signals:
  void layoutsChanged();
  void modelsChanged();
  void cachePathChanged();

private:
  QHash<QString, KKeyboardModel *> m_models;
  mutable QList<KKeyboardModel *> m_modelList;
  mutable QList<KKeyboardLayout *> m_layoutList;
  QHash<QString, KKeyboardLayout *> m_layouts;
  QString m_cachePath;
  const char *m_evdevPath = "/usr/share/X11/xkb/rules/evdev.xml";

  bool rebuildLayouts();
  void traverseXmlNodes(xmlNodeSetPtr nodes);
  void saveToJsonCache();
};
} // namespace myqmlplugin
