#pragma once

#include "formcontroller.h"
#include "forminput.h"
#include <qcontainerfwd.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmlparserstatus.h>
#include <qtmetamacros.h>

namespace mscp {
class AutoFormField : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(mscp::FormController *controller READ controller WRITE
                 setController NOTIFY controllerChanged)
  Q_PROPERTY(
      QStringList field READ field WRITE setField NOTIFY fieldChanged REQUIRED)

public:
  explicit AutoFormField(QObject *parent = nullptr);

  void classBegin() override {};
  void componentComplete() override;

  [[nodiscard]] QStringList field() const;
  void setField(const QStringList &value);

  [[nodiscard]] FormController *controller() const;
  void setController(FormController *controller);

signals:
  void fieldChanged();
  void controllerChanged();

private:
  QStringList m_field;
  FormController *m_controller;
  FormInput *m_formInput = nullptr;

  void attemptAttachField();
  void checkValidity();
};
} // namespace mscp
