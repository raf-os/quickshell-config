#pragma once

#include "cserializable.h"
#include "fieldcontroller.h"
#include <qlist.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qvariant.h>

namespace mscp {
class FormController : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QList<myqmlplugin::configs::CSerializable *> models READ models
                 WRITE setModels NOTIFY modelsChanged REQUIRED)
  Q_PROPERTY(QQmlListProperty<mscp::FieldController> fields READ fields NOTIFY
                 fieldsChanged)
  Q_PROPERTY(
      bool validationError READ validationError NOTIFY validationErrorChanged)

public:
  explicit FormController(QObject *parent = nullptr);

  struct FormAssignment {
    QObject *model;
    QMetaProperty prop;
    QVariant val;
    bool isValid = false;
  };

  [[nodiscard]] QList<myqmlplugin::configs::CSerializable *> models() const;
  void setModels(QList<myqmlplugin::configs::CSerializable *> models);

  [[nodiscard]] bool validationError() const;

  [[nodiscard]] QQmlListProperty<FieldController> fields();

  Q_INVOKABLE void validate();

signals:
  void fieldsChanged();
  void modelsChanged();
  void validationErrorChanged();
  void validationComplete();

private:
  QList<FieldController *> m_fields;
  bool m_validationError;
  QList<myqmlplugin::configs::CSerializable *> m_models;

  void modelParseProperties();
};
} // namespace mscp
