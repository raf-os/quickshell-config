#pragma once

#include "fieldcontroller.h"
#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace mscp {
class FormController : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(
      QObject *model READ model WRITE setModel NOTIFY modelChanged REQUIRED)

public:
  explicit FormController(QObject *parent = nullptr);

  [[nodiscard]] QObject *model() const;
  void setModel(QObject *model);

  Q_INVOKABLE void registerField(FieldController *field);
  Q_INVOKABLE void unregisterField(FieldController *field);

signals:
  void modelChanged();

private:
  QList<FieldController *> m_fields;
  QObject *m_model;
};
} // namespace mscp
