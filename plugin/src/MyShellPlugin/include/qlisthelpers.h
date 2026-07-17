#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qtypes.h>

template <typename T>
QQmlListProperty<T> readonlyQmlList(QObject    *o,
                                    QList<T *> *list) {
  return QQmlListProperty<T>(
      o,
      list,
      [](QQmlListProperty<T> *p) {
        return static_cast<QList<T *> *>(p->data)->size();
      },
      [](QQmlListProperty<T> *p, qsizetype idx) {
        return static_cast<QList<T *> *>(p->data)->at(idx);
      });
}
