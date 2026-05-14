#pragma once

#include "fieldcontroller.h"
#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace mscp {
class FormController : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(
      QObject *model READ model WRITE setModel NOTIFY modelChanged REQUIRED)
  Q_PROPERTY(
      QQmlListProperty<FieldController> fields READ fields NOTIFY fieldsChanged)
  Q_PROPERTY(
      bool validationError READ validationError NOTIFY validationErrorChanged)

public:
  explicit FormController(QObject *parent = nullptr);

  [[nodiscard]] QObject *model() const;
  void setModel(QObject *model);

  [[nodiscard]] bool validationError() const;

  [[nodiscard]] QQmlListProperty<FieldController> fields();

  Q_INVOKABLE void validate();

signals:
  void fieldsChanged();
  void modelChanged();
  void validationErrorChanged();
  void validationComplete();

private:
  QList<FieldController *> m_fields;
  bool m_validationError;
  QObject *m_model = nullptr;

  void modelParseProperties();
};
} // namespace mscp
