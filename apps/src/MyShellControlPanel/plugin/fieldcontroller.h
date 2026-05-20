#pragma once

#include "validator.h"

#include <optional>
#include <qjsvalue.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qvariant.h>

namespace mscp {
class FieldController : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString className READ className CONSTANT)
  Q_PROPERTY(QJSValue onValidation READ onValidation WRITE setOnValidation
                 NOTIFY validationChanged)
  Q_PROPERTY(QString validationError READ validationError NOTIFY
                 validationErrorChanged)
  Q_PROPERTY(bool isDirty READ isDirty WRITE setIsDirty NOTIFY isDirtyChanged)
  Q_PROPERTY(mscp::validators::Validator *cValidator READ cValidator WRITE
                 setCValidator NOTIFY cValidatorChanged)
public:
  explicit FieldController(QObject *reference, const QString &name,
                           const QVariant &initialValue,
                           QObject *parent = nullptr);

  [[nodiscard]] QString validationError() const;
  void setValidationError(const QString &value);

  [[nodiscard]] bool isDirty() const;
  void setIsDirty(bool value);

  [[nodiscard]] QVariant value() const;
  void setValue(const QVariant &value);

  [[nodiscard]] validators::Validator *cValidator() const;
  void setCValidator(validators::Validator *validator);

  [[nodiscard]] QString name() const;
  [[nodiscard]] QString className() const;

  [[nodiscard]] QJSValue onValidation() const;
  void setOnValidation(const QJSValue &value);

  QObject *getReference();

  std::optional<QVariant> triggerValidation(bool *validationError = nullptr);

signals:
  void valueChanged();
  void nameChanged();
  void validationChanged();
  void validationErrorChanged();
  void isDirtyChanged(bool isDirty);
  void cValidatorChanged();

private:
  validators::Validator *m_cValidator = nullptr;
  QObject *m_reference;
  QVariant m_value;
  QString m_name;
  QString m_className = "UNKNOWN_CLASS";
  QJSValue m_validator;
  QString m_validationError;
  bool m_isDirty = false;
};
} // namespace mscp
