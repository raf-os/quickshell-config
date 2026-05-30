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
  Q_PROPERTY(bool isDirty READ isDirty NOTIFY isDirtyChanged)

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
  [[nodiscard]] bool isDirty() const;
  void setIsDirty(bool value);

  [[nodiscard]] QQmlListProperty<FieldController> fields();

  Q_INVOKABLE void commitValues();
  Q_INVOKABLE void resetForm();
  Q_INVOKABLE FieldController *fieldFor(const QString &className,
                                        const QString &propName);

signals:
  void fieldsChanged();
  void modelsChanged();
  void validationErrorChanged();
  void validationComplete();
  void isDirtyChanged();

private:
  QList<FieldController *> m_fields;
  bool m_validationError;
  bool m_isDirty;
  QList<myqmlplugin::configs::CSerializable *> m_models;

  void modelParseProperties();
  void delegateSetIsDirty(bool value);
};
} // namespace mscp
