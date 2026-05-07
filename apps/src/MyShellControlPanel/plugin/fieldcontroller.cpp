#include "fieldcontroller.h"
#include "formcontroller.h"
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
} // namespace mscp
