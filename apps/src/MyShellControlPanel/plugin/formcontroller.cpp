#include "formcontroller.h"
#include "fieldcontroller.h"
#include <qobject.h>

namespace mscp {
FormController::FormController(QObject *parent) : QObject(parent) {}

void FormController::registerField(FieldController *field) {
  m_fields.append(field);
}

void FormController::unregisterField(FieldController *field) {
  auto idx = m_fields.indexOf(field);
  if (idx > 0) {
    m_fields.removeAt(idx);
  }
}

QObject *FormController::model() const { return m_model; }

void FormController::setModel(QObject *model) {
  if (m_model != model) {
    m_model = model;
    emit modelChanged();
  }
}
} // namespace mscp
