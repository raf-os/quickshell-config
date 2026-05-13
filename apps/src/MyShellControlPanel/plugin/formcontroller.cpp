#include "formcontroller.h"
#include "fieldcontroller.h"
#include <algorithm>
#include <qlogging.h>
#include <qmetaobject.h>
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

bool FormController::validationError() const { return m_validationError; }

QObject *FormController::model() const { return m_model; }

void FormController::setModel(QObject *model) {
  if (m_model != model) {
    m_model = model;
    emit modelChanged();
  }
}

void FormController::validate() {
  if (m_model == nullptr) {
    qWarning() << "mscp::FormController::validate: No model to assign "
                  "validated values to.";
  }
  bool validationError = false;
  auto modelMetaObj = m_model->metaObject();
  for (const auto field : m_fields) {
    auto result = field->triggerValidation(&validationError);
    if (validationError)
      break;
    if (!result.has_value())
      continue;

    auto propId = modelMetaObj->indexOfProperty(field->name().toLocal8Bit());
    if (propId == -1 || propId < modelMetaObj->superClass()->propertyCount()) {
      qWarning() << "mscp::FormController::validate: Invalid form name "
                    "property, skipping...";
      continue;
    } else {
      QMetaProperty prop = modelMetaObj->property(propId);
      if (prop.isWritable()) {
        prop.write(m_model, std::move(result.value()));
      }
    }
  }

  if (validationError) {
    m_validationError = true;
    emit validationErrorChanged();
  }

  emit validationComplete();
}
} // namespace mscp
