#include "fieldcontroller.h"
#include "validator.h"
#include "validatorresponse.h"

#include <optional>
#include <qjsengine.h>
#include <qjsvalue.h>
#include <qlogging.h>
#include <qmetaobject.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qsharedpointer.h>
#include <qvariant.h>

namespace mscp {
FieldController::FieldController(QObject *reference, const QString &name,
                                 const QVariant &initialValue, QObject *parent)
    : QObject(parent), m_reference(reference), m_name(name),
      m_value(initialValue) {
  auto mo = m_reference->metaObject();
  auto mpidx = mo->indexOfProperty(m_name.toUtf8());
  auto cm = mo->indexOfMethod("getClassName()");

  if (cm != -1) {
    auto m = mo->method(cm);
    QString retVal;
    m.invoke(m_reference, Qt::DirectConnection, qReturnArg(retVal));
    if (retVal != "") {
      m_className = retVal;
    }
  }

  if (mpidx == -1) {
    qWarning() << "mscp::FormMetaModel: Can't find property " << m_name
               << " on reference object!";
    deleteLater();
    return;
  }

  auto mprop = mo->property(mpidx);
  auto to = metaObject();
  auto tprop = to->property(to->indexOfProperty("value"));

  QObject::connect(m_reference, mprop.notifySignal(), this,
                   tprop.notifySignal());

  QObject::connect(m_reference, &QObject::destroyed, this, [this]() {
    m_reference = nullptr;
    deleteLater();
  });
}

QObject *FieldController::getReference() { return m_reference; }

QVariant FieldController::value() const { return m_value; }

void FieldController::setValue(const QVariant &value) {
  if (m_value != value) {
    m_value = value;
    emit valueChanged();
  }
}

QString FieldController::name() const { return m_name; }
QString FieldController::className() const { return m_className; }

bool FieldController::isDirty() const { return m_isDirty; }

void FieldController::setIsDirty(bool value) {
  if (m_isDirty != value) {
    m_isDirty = value;
    emit isDirtyChanged(value);
  }
}

QJSValue FieldController::onValidation() const { return m_validator; }

void FieldController::setOnValidation(const QJSValue &value) {
  if (!m_validator.strictlyEquals(value)) {
    m_validator = value;
    emit validationChanged();
  }
}

validators::Validator *FieldController::cValidator() const {
  return m_cValidator;
}

void FieldController::setCValidator(validators::Validator *validator) {
  if (m_cValidator != validator) {
    m_cValidator = validator;
    emit cValidatorChanged();
  }
}

QString FieldController::validationError() const { return m_validationError; }

void FieldController::setValidationError(const QString &value) {
  if (m_validationError != value) {
    m_validationError = value;
    emit validationChanged();
  }
}

std::optional<QVariant>
FieldController::triggerValidation(bool *validationError) {
  QVariant finalValue = m_value;

  if (!m_isDirty) {
    return std::nullopt;
  }

  if (m_cValidator != nullptr) {
    validators::ValidatorResponse res;
    m_cValidator->validate(finalValue, &res);

    if (res.success == false) {
      if (validationError)
        *validationError = false;
      setValidationError(res.message);
      return std::nullopt;
    }

    finalValue = res.value;
  }

  if (m_validator.isNull() || m_validator.isUndefined()) {
    // No validation function was defined on the QML side
    return finalValue;
  }

  QJSEngine *engine = qjsEngine(this);
  if (!engine)
    return finalValue;

  if (!m_validator.isCallable()) {
    return m_validator.toVariant();
  }

  QJSValue result = m_validator.call({finalValue.toString()});

  if (result.isError()) {
    qWarning() << "mscp::FieldController::triggerValidation: Javascript error "
                  "on validating field "
               << m_name << ": " << result.toString();
    setValidationError("Internal error occurred.");
    if (validationError) {
      *validationError = true;
    }
    return std::nullopt;
  }

  if (result.isArray()) {
    qWarning() << "mscp::FieldController::triggerValidation: Received invalid "
                  "validation value for field '"
               << m_name << "'.";
    setValidationError("Internal error occurred.");
    if (validationError)
      *validationError = true;
    return std::nullopt;
  }

  if (result.isNull() || result.isUndefined()) {
    return std::nullopt;
  } else if (result.isObject()) {
    auto response = result.toVariant().toMap();
    auto succ = response.constFind("success");

    if (succ == response.constEnd()) {
      return std::nullopt;
    } else {
      auto success = succ->toBool();
      auto value = response.constFind("value");
      auto msg = response.constFind("message");

      if (success) {
        if (value == response.constEnd()) {
          setValidationError(
              "Invalid validator - check logs for error message.");
          qWarning() << "mscp::FieldController::triggerValidation: Field '"
                     << m_name << "' returned invalid JSON.";
          if (validationError)
            *validationError = true;
          return std::nullopt;
        }
        setValidationError("");
        return std::nullopt;
      } else {
        if (msg == response.constEnd()) {
          setValidationError("Unknown validation error occurred.");
          if (validationError)
            *validationError = true;
          return std::nullopt;
        } else {
          setValidationError(msg->toString());
          if (validationError)
            *validationError = true;
          return std::nullopt;
        }
      }
    }
  } else {
    setValidationError("Internal error occurred.");
    if (validationError)
      *validationError = true;
    return std::nullopt;
  }
}
} // namespace mscp
