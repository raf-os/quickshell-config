#pragma once

#include <map>
#include <memory>
#include <string>

#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qsortfilterproxymodel.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

#include "keyboard_layout_serializer.h"
#include "ns_utils_export.h"

namespace ns::utils {
class KbdNode {
public:
  explicit KbdNode(KbdNode *parent = nullptr);

  QString name;
  QString variant;

  [[nodiscard]] KbdNode *parent();
  [[nodiscard]] KbdNode *child(int row);
  [[nodiscard]] int      childCount() const;
  [[nodiscard]] int      row() const;

  void appendChild(const std::string &name, std::unique_ptr<KbdNode> &&child);

private:
  std::map<std::string, std::unique_ptr<KbdNode>> m_children;
  KbdNode                                        *m_parent;

  friend class KbdLayoutsModel;
};

class NS_UTILS_EXPORT KbdLayoutsModel : public QAbstractItemModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(KeyboardLayoutsModel)

public:
  explicit KbdLayoutsModel(KeyboardLayoutXMLSerializer *parent = nullptr);
  // needed for unique_ptr
  ~KbdLayoutsModel() override;
  Q_DISABLE_COPY_MOVE(KbdLayoutsModel)

  enum Roles { Name = Qt::UserRole, Variant, Description, ShortDescription };

  QHash<int, QByteArray> roleNames() const override;
  int                    rowCount(const QModelIndex &parent) const override;
  int                    columnCount(const QModelIndex &parent) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QModelIndex index(
      int row, int column, const QModelIndex &parent) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant    data(const QModelIndex &index, int role) const override;

private slots:
  void parseData();

private:
  std::unique_ptr<KbdNode> m_root;

  KeyboardLayoutXMLSerializer *m_serializer;
};
} // namespace ns::utils
