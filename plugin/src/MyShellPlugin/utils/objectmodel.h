#pragma once

#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>
namespace ns::utils {
class UntypedObjectModel : public QAbstractListModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(ObjectListModel)
  QML_UNCREATABLE("")

  Q_PROPERTY(QList<QObject *> values READ values NOTIFY valuesChanged)

public:
  explicit UntypedObjectModel(QObject *parent) : QAbstractListModel(parent) {}

  [[nodiscard]] QHash<int, QByteArray>   roleNames() const override;
  [[nodiscard]] virtual QList<QObject *> values()                       = 0;
  Q_INVOKABLE virtual qsizetype          indexOf(QObject *object) const = 0;
};

template <typename T> class ObjectModel : public UntypedObjectModel {
public:
  explicit ObjectModel(QObject *parent) : UntypedObjectModel(parent) {}

  [[nodiscard]] const QList<T *> &valueList() const { return m_values; }
  [[nodiscard]] QList<T *>       &valueList() { return m_values; }

  [[nodiscard]] qint32 rowCount(const QModelIndex &parent) const override {
    if (parent != QModelIndex()) return 0;
    return static_cast<qint32>(m_values.length());
  }

  [[nodiscard]] QVariant data(const QModelIndex &index,
                              qint32             role) const override {
    if (!index.isValid() || role != Qt::UserRole + 1) return QVariant();
    // TODO: Forward declarations would break static_cast, look into this
    return QVariant::fromValue(
        reinterpret_cast<QObject *>(m_values.at(index.row())));
  }

  qsizetype indexOf(QObject *object) const override {
    return m_values.indexOf(reinterpret_cast<T *>(object));
  }

  [[nodiscard]] QList<QObject *> values() override {
    return *reinterpret_cast<QList<QObject *> *>(&m_values);
  }

private:
  QList<T *> m_values;
};
} // namespace ns::utils
