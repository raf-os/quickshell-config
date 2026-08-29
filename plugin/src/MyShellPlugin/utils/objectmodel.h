#pragma once

#include <functional>
#include <iterator>

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

signals:
  void valuesChanged();
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

  [[nodiscard]] QVariant data(
      const QModelIndex &index, qint32 role) const override {
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

  void insertObject(T *object, qsizetype index) {
    const auto intIdx = static_cast<qint32>(index);

    beginInsertRows(QModelIndex(), intIdx, intIdx);
    m_values.insert(index, object);
    endInsertRows();

    emit valuesChanged();
  }

  void insertObject(T *object) {
    return insertObject(object, m_values.length());
  }

  void insertObjectSorted(
      T *object, const std::function<bool(T *, T *)> &compare) {
    auto it = m_values.begin();
    while (it != m_values.end()) {
      if (!compare(object, *it)) break;
      ++it;
    }

    auto idx = std::distance(m_values.begin(), it);
    insertObject(object, idx);
  }

  void removeAt(qsizetype index) {
    auto intIdx = static_cast<qint32>(index);
    beginRemoveRows(QModelIndex(), intIdx, intIdx);
    m_values.removeAt(index);
    endRemoveRows();

    emit valuesChanged();
  }

  bool removeObject(const T *object) {
    auto idx = m_values.indexOf(object);
    if (idx == -1) return false;

    removeAt(idx);
    return true;
  }

  bool diffUpdate(const QList<T *> &newValues) {
    // First, remove elements that are not present
    auto it = m_values.begin();
    while (it != m_values.end()) {
      if (newValues.contains(*it)) {
        it++;
      } else {
        const auto idx = std::distance(m_values.begin(), it);
        removeAt(idx);
      }
    }

    // Then, add new objects and reorganize existing ones if necessary
    qsizetype i = 0;
    for (auto *object : newValues) {
      if (m_values.length() == i || m_values.at(i) != object) {
        auto old = m_values.indexOf(object, i);
        if (old != -1) {
          // This means this object already exists further down the line
          beginMoveRows({}, old, i);
          m_values.move(old, i);
          endMoveRows();
          emit valuesChanged();
        } else {
          // It's a new object
          insertObject(object, i);
        }
      }

      i++;
    }
  }

private:
  QList<T *> m_values;
};
} // namespace ns::utils
