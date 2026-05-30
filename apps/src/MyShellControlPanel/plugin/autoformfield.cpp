#include "autoformfield.h"
#include "formcontroller.h"
#include "forminput.h"

#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qobject.h>

namespace mscp {
AutoFormField::AutoFormField(QObject *parent) : QObject(parent) {}

void AutoFormField::componentComplete() { checkValidity(); }

void AutoFormField::checkValidity() {
  m_formInput = qobject_cast<FormInput *>(parent());

  if (m_formInput == nullptr) {
    qWarning() << "mscp::AutoFormField: Parent class MUST be a FormInput!!!";
    deleteLater();
    return;
  }

  attemptAttachField();
}

FormController *AutoFormField::controller() const { return m_controller; }
void AutoFormField::setController(FormController *controller) {
  if (controller == m_controller)
    return;

  if (controller != nullptr) {
    QObject::disconnect(controller, nullptr, this, nullptr);
  }

  m_controller = controller;
  emit controllerChanged();

  attemptAttachField();
}

QStringList AutoFormField::field() const { return m_field; }
void AutoFormField::setField(const QStringList &value) {
  if (value == m_field)
    return;

  m_field = value;
  emit fieldChanged();

  attemptAttachField();
}

void AutoFormField::attemptAttachField() {
  if (m_controller == nullptr || m_formInput == nullptr || m_field.isEmpty())
    return;

  if (m_field.length() != 2) {
    return;
  }

  const auto fptr = m_controller->fieldFor(m_field[0], m_field[1]);

  if (!fptr) {
    qWarning() << "mscp::AutoFormField: Selected field does not exist as a "
                  "child of FormController!";
    return;
  }

  m_formInput->setController(fptr);
}
} // namespace mscp
