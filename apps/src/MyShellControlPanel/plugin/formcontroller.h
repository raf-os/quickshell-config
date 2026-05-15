#pragma once

#include "cserializable.h"
#include "fieldcontroller.h"
#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace mscp {
class FormController : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(myqmlplugin::configs::CSerializable *model READ model WRITE
                 setModel NOTIFY modelChanged REQUIRED)
  Q_PROPERTY(QQmlListProperty<mscp::FieldController> fields READ fields NOTIFY
                 fieldsChanged)
  Q_PROPERTY(
      bool validationError READ validationError NOTIFY validationErrorChanged)

public:
  explicit FormController(QObject *parent = nullptr);

  [[nodiscard]] myqmlplugin::configs::CSerializable *model() const;
  void setModel(myqmlplugin::configs::CSerializable *model);

  [[nodiscard]] bool validationError() const;

  [[nodiscard]] QQmlListProperty<FieldController> fields();

  Q_INVOKABLE void validate();

signals:
  void fieldsChanged();
  void modelChanged();
  void validationErrorChanged();
  void validationComplete();

private:
  QList<FieldController *> m_fields;
  bool m_validationError;
  myqmlplugin::configs::CSerializable *m_model = nullptr;

  void modelParseProperties();
};
} // namespace mscp
