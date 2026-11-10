#pragma once

#include <optional>

#include <qcontainerfwd.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qmap.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qxmlstream.h>

namespace ns::utils {
struct KConfigItem {
  QString name;
  QString shortDescription;
  QString description;
};

struct KLayout {
  KConfigItem                item;
  QMap<QString, KConfigItem> variants;
};

struct KLayoutMetadata {
  QString name;
  QString variant = {};
  QString description;
  QString shortDescription;
};

using KConfigTable = QMap<QString, KLayout>;

class KeyboardLayoutXMLSerializer : public QObject {
  Q_OBJECT

public:
  explicit KeyboardLayoutXMLSerializer(
      const char *xmlPath = nullptr, QObject *parent = nullptr);

  [[nodiscard]] KConfigTable   getLayoutTable() const;
  [[nodiscard]] const KLayout *getLayoutByName(const QString &name) const;
  [[nodiscard]] std::optional<KLayoutMetadata> getConfigItem(
      const QString &name, const QString &variant = {}) const;

public slots:
  bool serialize(const QString &path);

signals:
  void ready();

private:
  void        readEvdevBody(QXmlStreamReader &xml);
  void        readLayoutBody(QXmlStreamReader &xml);
  KConfigItem readConfigItem(QXmlStreamReader &xml);

  KConfigTable m_layouts;
};
} // namespace ns::utils
