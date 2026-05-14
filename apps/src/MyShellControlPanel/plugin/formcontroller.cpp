#include "formcontroller.h"
#include "fieldcontroller.h"
#include <qlist.h>
#include <qlogging.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qvariant.h>
#include <utility>

namespace mscp {
FormController::FormController(QObject *parent) : QObject(parent) {}

void FormController::modelParseProperties() {
  if (m_model == nullptr)
    return;

  for (const auto field : m_fields) {
    field->deleteLater();
  }

  m_fields.clear();

  auto metaObject = m_model->metaObject();

  for (auto i = metaObject->superClass()->propertyCount();
       i < metaObject->propertyCount(); i++) {
    auto prop = metaObject->property(i);

    if (!prop.isReadable())
      continue;

    auto propName = prop.name();
    QString propValue = prop.read(m_model).toString();

    auto field = new FieldController(m_model, QString::fromUtf8(propName),
                                     propValue, this);

    QObject::connect(field, &QObject::destroyed, this, [this, field]() {
      auto idx = m_fields.indexOf(field);
      if (idx != -1) {
        m_fields.removeAt(idx);
        emit fieldsChanged();
      }
    });

    field->setValue(propValue);

    m_fields.append(field);
  }

  emit fieldsChanged();
}

bool FormController::validationError() const { return m_validationError; }

QObject *FormController::model() const { return m_model; }

void FormController::setModel(QObject *model) {
  if (m_model != model) {
    m_model = model;
    emit modelChanged();
    if (m_model != nullptr)
      modelParseProperties();
  }
}

QQmlListProperty<FieldController> FormController::fields() {
  return QQmlListProperty<FieldController>(this, &m_fields);
}

void FormController::validate() {
  if (m_model == nullptr) {
    qWarning() << "mscp::FormController::validate: No model to assign "
                  "validated values to.";
    return;
  }

  QList<std::pair<QMetaProperty, QVariant>> assignments;

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
        assignments.append(
            std::make_pair(std::move(prop), std::move(result.value())));
        // prop.write(m_model, std::move(result.value()));
      }
    }
  }

  if (validationError) {
    m_validationError = true;
    emit validationErrorChanged();
  } else {
    if (m_validationError == true) {
      m_validationError = false;
      emit validationErrorChanged();
    }

    for (const auto assignment : assignments) {
      assignment.first.write(m_model, std::move(assignment.second));
    }
  }

  emit validationComplete();
}
} // namespace mscp
