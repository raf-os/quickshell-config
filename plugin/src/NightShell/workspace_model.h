#pragma once

#include <memory>

#include <QAbstractItemModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QtQmlIntegration>

#include "shell_global.h"

namespace ns {
class WorkspaceModelPrivate;

class WorkspaceModel : public QAbstractItemModel {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(ns::ShellGlobal *shellGlobal READ shellGlobal WRITE setShellGlobal
                 NOTIFY shellGlobalChanged)

public:
  explicit WorkspaceModel(QObject *parent = nullptr);
  ~WorkspaceModel() override;

  [[nodiscard]] ShellGlobal *shellGlobal();
  void                       setShellGlobal(ShellGlobal *value);

  [[nodiscard]] int rowCount(const QModelIndex &index) const override;
  [[nodiscard]] int columnCount(const QModelIndex &index) const override;

  [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
  [[nodiscard]] QModelIndex index(int row, int column,
                                  const QModelIndex &parent) const override;
  [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
  [[nodiscard]] QVariant    data(const QModelIndex &index,
                                 int                role) const override;

signals:
  void shellGlobalChanged();

private:
  std::unique_ptr<WorkspaceModelPrivate> d;

  ShellGlobal *m_shellGlobal = nullptr;
};
} // namespace ns
