#include "fieldcontroller.h"
#include "formcontroller.h"
#include <optional>
#include <qjsengine.h>
#include <qjsvalue.h>
#include <qlogging.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qvariant.h>

namespace mscp {
FieldController::FieldController(QObject *parent) : QObject(parent) {}

FieldController::~FieldController() {
  if (!m_controller)
    return;
  auto cast = qobject_cast<FormController *>(m_controller);
  if (cast) {
    cast->unregisterField(this);
  }
}

QObject *FieldController::controller() const { return m_controller; }

void FieldController::setController(QObject *controller) {
  auto cast = qobject_cast<FormController *>(controller);
  if (cast && cast != m_controller) {
    cast->registerField(this);
    m_controller = cast;
    QObject::connect(m_controller, &QObject::destroyed, this,
                     [this] { deleteLater(); });
    emit controllerChanged();
  }
}

QVariant FieldController::value() const { return m_value; }

void FieldController::setValue(const QVariant &value) {
  if (m_value != value) {
    m_value = value;
    emit valueChanged();
  }
}

QString FieldController::name() const { return m_name; }

void FieldController::setName(const QString &value) {
  if (m_name != value) {
    m_name = value;
    emit nameChanged();
  }
}

bool FieldController::isDirty() const { return m_isDirty; }

void FieldController::setIsDirty(const bool &value) {
  if (m_isDirty != value) {
    m_isDirty = value;
    emit isDirtyChanged();
  }
}

QJSValue FieldController::onValidation() const { return m_validator; }

void FieldController::setOnValidation(const QJSValue &value) {
  if (!m_validator.strictlyEquals(value)) {
    m_validator = value;
    emit validationChanged();
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
  if (!m_isDirty) {
    return std::nullopt;
  }

  if (m_validator.isNull() || m_validator.isUndefined()) {
    // No validation function was defined on the QML side
    return m_value;
  }

  QJSEngine *engine = qjsEngine(this);
  if (!engine)
    return m_value;

  if (!m_validator.isCallable()) {
    return m_validator.toVariant();
  }

  QJSValue result = m_validator.call();

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
