#pragma once

#include <optional>

#include <qcontainerfwd.h>
#include <qhash.h>
#include <qjsengine.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qxmlstream.h>

namespace ns::utils {
struct SLayoutMetadata {
  QString name;
  QString variant;
  QString description;
};

struct SKeyboardVariant {
  QStringList languageList;
  QString     name;
  QString     description;
  QString     shortDescription;
};

struct SKeyboardLayout {
  QList<SKeyboardVariant> variants;
  QStringList             countryList;
  QStringList             languageList;
  QString                 name;
  QString                 shortDescription;
  QString                 description;
};

class CKeyboardVariant : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)
  Q_PROPERTY(QString shortDescription READ shortDescription CONSTANT)
  Q_PROPERTY(QStringList languageList READ languageList CONSTANT)

public:
  explicit CKeyboardVariant(SKeyboardVariant payload,
                            QObject         *parent = nullptr);

  [[nodiscard]] QString     name() const;
  [[nodiscard]] QString     description() const;
  [[nodiscard]] QString     shortDescription() const;
  [[nodiscard]] QStringList languageList() const;

private:
  SKeyboardVariant data;
};

class CKeyboardLayout : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString shortDescription READ shortDescription CONSTANT)
  Q_PROPERTY(QString description READ description CONSTANT)
  Q_PROPERTY(QStringList countryList READ countryList CONSTANT)
  Q_PROPERTY(QStringList languageList READ languageList CONSTANT)
  Q_PROPERTY(QQmlListProperty<CKeyboardVariant> variants READ variants NOTIFY
                 variantsChanged)

public:
  explicit CKeyboardLayout(SKeyboardLayout payload,
                           QObject        *parent = nullptr);

  [[nodiscard]] QString     name() const;
  [[nodiscard]] QString     shortDescription() const;
  [[nodiscard]] QString     description() const;
  [[nodiscard]] QStringList countryList() const;
  [[nodiscard]] QStringList languageList() const;

  QQmlListProperty<CKeyboardVariant> variants();
  QList<CKeyboardVariant *>          variantList() const;
  void setVariants(QList<SKeyboardVariant> &variants);

signals:
  void variantsChanged();

private:
  SKeyboardLayout                    data;
  QHash<QString, CKeyboardVariant *> m_variants;
  QList<CKeyboardVariant *>          m_variantList;
};

class KeyboardLayouts : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(QQmlListProperty<CKeyboardLayout> layouts READ layouts CONSTANT)

public:
  static KeyboardLayouts *instance() {
    static KeyboardLayouts *s_instance = new KeyboardLayouts();
    return s_instance;
  }

  static KeyboardLayouts *create(QQmlEngine *qmlEngine,
                                 QJSEngine * /* unused */) {
    auto inst = instance();
    if (qmlEngine)
      qmlEngine->setObjectOwnership(inst, QQmlEngine::CppOwnership);
    return inst;
  }

  std::optional<SLayoutMetadata> fetchLayoutMetadata(QStringView layout,
                                                     QStringView variant);

  QQmlListProperty<CKeyboardLayout> layouts();
  QList<CKeyboardLayout *>          getSortedLayoutList() const;

private:
  explicit KeyboardLayouts(QObject *parent = nullptr);

  QHash<QString, CKeyboardLayout *> m_layouts;
  QList<CKeyboardLayout *>          m_sortedLayoutList;
  const char *m_evdevPath = "/usr/share/X11/xkb/rules/evdev.xml";

  void buildDatabase();

  void readXmlBody(QXmlStreamReader &xml);
  void readXmlLayoutList(QXmlStreamReader &xml);
  void readXmlConfigItem(QXmlStreamReader &xml,
                         SKeyboardLayout  &layout);
  void readXmlVariantList(QXmlStreamReader &xml,
                          SKeyboardLayout  &layout);
};
} // namespace ns::utils
