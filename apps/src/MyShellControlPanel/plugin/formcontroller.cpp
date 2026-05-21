#include "formcontroller.h"
#include "cserializable.h"
#include "fieldcontroller.h"
#include "iconfigserializer.h"

#include <qlist.h>
#include <qlogging.h>
#include <qmetaobject.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qqmllist.h>
#include <qsharedpointer.h>
#include <qvariant.h>
#include <utility>

namespace mscp {
FormController::FormController(QObject *parent) : QObject(parent) {}

void FormController::modelParseProperties() {
  if (m_models.isEmpty())
    return;

  for (const auto field : m_fields) {
    field->deleteLater();
  }

  m_fields.clear();

  for (auto const m_model : m_models) {
    if (m_model == nullptr) {
      continue;
    }

    auto metaObject = m_model->metaObject();

    for (auto i = metaObject->superClass()->propertyCount();
         i < metaObject->propertyCount(); i++) {
      auto prop = metaObject->property(i);

      // Skip unreadable and strictly bound properties
      if (!prop.isReadable() || !prop.isStored())
        continue;

      QMetaType metaType = prop.metaType();

      // Skip nested objects
      if (metaType.flags().testFlag(QMetaType::PointerToQObject))
        continue;

      auto propName = prop.name();
      QVariant propValue = prop.read(m_model);

      FieldController *field = new FieldController(
          m_model, QString::fromUtf8(propName), propValue, this);

      QObject::connect(field, &QObject::destroyed, this, [this, field]() {
        auto idx = m_fields.indexOf(field);
        if (idx != -1) {
          m_fields.removeAt(idx);
          emit fieldsChanged();
        }
      });

      QObject::connect(field, &FieldController::isDirtyChanged, this,
                       &FormController::setIsDirty);

      field->setValue(propValue);

      m_fields.append(field);
    }
  }

  emit fieldsChanged();
}

bool FormController::validationError() const { return m_validationError; }

bool FormController::isDirty() const { return m_isDirty; }

void FormController::setIsDirty(bool value) {
  if (m_isDirty != value) {
    m_isDirty = value;
    emit isDirtyChanged();
  }
}

QList<myqmlplugin::configs::CSerializable *> FormController::models() const {
  return m_models;
}

void FormController::setModels(
    QList<myqmlplugin::configs::CSerializable *> models) {
  if (m_models != models) {
    m_models.clear();
    m_models = models;
    emit modelsChanged();
    modelParseProperties();
  }
}

QQmlListProperty<FieldController> FormController::fields() {
  return QQmlListProperty<FieldController>(this, &m_fields);
}

void FormController::validate() {
  if (m_models.isEmpty()) {
    qWarning() << "mscp::FormController::validate: No model to assign "
                  "validated values to.";
    return;
  }

  QList<FormAssignment> assignments(m_fields.length());

  bool validationError = false;
  for (int i = 0; i < m_fields.length(); ++i) {
    auto modelMetaObj = m_fields.at(i)->getReference()->metaObject();
    auto result = m_fields.at(i)->triggerValidation(&validationError);
    if (validationError)
      break;
    if (!result.has_value()) {
      continue;
    }

    auto propId =
        modelMetaObj->indexOfProperty(m_fields.at(i)->name().toLocal8Bit());
    if (propId == -1 || propId < modelMetaObj->superClass()->propertyCount()) {
      qWarning() << "mscp::FormController::validate: Invalid form name "
                    "property, skipping...";
      continue;
    } else {
      QMetaProperty prop = modelMetaObj->property(propId);
      if (prop.isWritable()) {
        assignments[i].isValid = true;
        assignments[i].model = m_fields.at(i)->getReference();
        assignments[i].prop = std::move(prop);
        assignments[i].val = m_fields.at(i)->value();
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
      if (assignment.isValid) {
        assignment.prop.write(assignment.model, assignment.val);
      }
    }

    for (const auto model : m_models) {
      auto root = model->getRoot();
      if (auto s =
              qobject_cast<myqmlplugin::configs::IConfigSerializer *>(root)) {
        s->commitSave();
      }
    }

    for (const auto field : m_fields) {
      field->setIsDirty(false);
    }

    setIsDirty(false);
  }

  emit validationComplete();
}

void FormController::resetForm() {
  for (const auto field : m_fields) {
    field->resetState(true);
  }
  emit fieldsChanged();
}
} // namespace mscp
