#include "forminput.h"
#include "fieldcontroller.h"
#include "validator.h"

#include <qjsvalue.h>
#include <qlogging.h>
#include <qobject.h>
#include <qquickitem.h>
#include <qvariant.h>

namespace mscp {
FormInput::FormInput(QQuickItem *parent) : QQuickItem(parent) {}

QString FormInput::name() const { return m_name; }
void FormInput::setName(const QString &value) {
  if (m_name != value) {
    m_name = value;
    emit nameChanged();
  }
}

QVariant FormInput::initialValue() const {
  if (m_controller)
    return m_controller->initialValue();
  else
    return QVariant();
}
void FormInput::setInitialValue() {
  if (!m_controller)
    return;

  auto value = initialValue();

  if (!m_inputItem) {
    m_initialValueBuffer = value;
    emit initialValueChanged();
    return;
  }

  writeToInputItem(value);
  emit initialValueChanged();
}

QObject *FormInput::inputItem() const { return m_inputItem; }
void FormInput::setInputItem(QObject *item) {
  if (item != m_inputItem) {
    if (m_inputItem) {
      QObject::disconnect(m_inputItem, nullptr, this, nullptr);
    }
    m_inputItem = item;

    if (m_inputItem) {
      QObject::connect(m_inputItem, &QObject::destroyed, this,
                       [this]() { this->m_inputItem = nullptr; });

      attemptConnectFields();
    }

    inputItemChanged();

    if (m_initialValueBuffer.isValid() && m_inputItem) {
      writeToInputItem(m_initialValueBuffer);
      m_initialValueBuffer.clear();
    }
  }
}

void FormInput::attemptConnectFields() {
  if (!m_controller || !m_inputItem)
    return;

  auto mo = m_inputItem->metaObject();
  auto pidx = mo->indexOfProperty("value");

  if (pidx == -1)
    return;

  auto mprop = mo->property(pidx);

  if (!mprop.isReadable())
    return;

  auto tmo = metaObject();
  auto tmethod = tmo->method(tmo->indexOfMethod("rerouteValue()"));

  QObject::connect(m_inputItem, mprop.notifySignal(), this, tmethod);
}

void FormInput::rerouteValue() {
  if (!m_controller || !m_inputItem)
    return;
  auto mo = m_inputItem->metaObject();
  auto mprop = mo->property(mo->indexOfProperty("value"));
  m_controller->setValue(mprop.read(m_inputItem));
};

bool FormInput::isValid() const { return m_isValid; }
void FormInput::setIsValid(const bool &value) {
  if (value != m_isValid) {
    m_isValid = value;
    emit isValidChanged();
  }
}

void FormInput::writeToInputItem(const QVariant &value) {
  if (!m_inputItem)
    return;

  auto metaObject = m_inputItem->metaObject();
  auto propIndex = metaObject->indexOfProperty("value");

  if (propIndex == -1)
    return;

  auto metaProp = metaObject->property(propIndex);

  if (!metaProp.isWritable())
    return;

  metaProp.write(m_inputItem, value);
}

validators::Validator *FormInput::cValidator() const {
  if (m_controller)
    return m_controller->cValidator();
  else
    return nullptr;
}
void FormInput::setCValidator(validators::Validator *validator) {
  if (m_controller)
    m_controller->setCValidator(validator);
}

FieldController *FormInput::controller() const { return m_controller; }
void FormInput::setController(FieldController *controller) {
  if (m_controller != controller) {
    if (m_controller)
      QObject::disconnect(m_controller, nullptr, this, nullptr);

    if (!controller) {
      m_controller = nullptr;
      emit controllerChanged();
    }

    m_controller = controller;
    auto mo = m_controller->metaObject();
    auto tmo = metaObject();

    const auto props = {"value", "isDirty", "onValidation", "validationError",
                        "cValidator"};

    // Mirror relevant props
    for (const auto &prop : props) {
      auto metaProp = mo->property(mo->indexOfProperty(prop));
      auto thisMetaProp = tmo->property(tmo->indexOfProperty(prop));
      QObject::connect(m_controller, metaProp.notifySignal(), this,
                       thisMetaProp.notifySignal());
    }

    QObject::connect(m_controller, &FieldController::initialValueChanged, this,
                     &FormInput::setInitialValue);
    QObject::connect(m_controller, &FieldController::validationErrorChanged,
                     this, [this]() {
                       if (m_controller->validationError().isEmpty()) {
                         setIsValid(true);
                       } else {
                         setIsValid(false);
                       }
                     });

    QObject::connect(m_controller, &QObject::destroyed, this,
                     [this]() { this->m_controller = nullptr; });

    setInitialValue();
    emit controllerChanged();

    attemptConnectFields();
  }
}
} // namespace mscp
