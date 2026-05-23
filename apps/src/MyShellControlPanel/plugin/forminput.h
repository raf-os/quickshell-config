#pragma once

#include "fieldcontroller.h"
#include "validator.h"

#include <qjsvalue.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qquickitem.h>
#include <qtmetamacros.h>
#include <qvariant.h>

#define AUTO_PROP_REROUTE_IMPL_DECLARE(Type, Name, NameCap)                    \
  Q_PROPERTY(Type Name READ Name WRITE set##NameCap NOTIFY Name##Changed)      \
public:                                                                        \
  void set##NameCap(const Type &value) {                                       \
    if (m_controller)                                                          \
      m_controller->set##NameCap(value);                                       \
  }                                                                            \
  Q_SIGNAL void Name##Changed();

#define AUTO_PROP_REROUTE(Type, Name, NameCap)                                 \
  AUTO_PROP_REROUTE_IMPL_DECLARE(Type, Name, NameCap)                          \
public:                                                                        \
  Type Name() const {                                                          \
    if (m_controller) {                                                        \
      return m_controller->Name();                                             \
    } else {                                                                   \
      return Type();                                                           \
    }                                                                          \
  }

#define AUTO_PROP_REROUTE_DEFAULT(Type, Name, NameCap, Default)                \
  AUTO_PROP_REROUTE_IMPL_DECLARE(Type, Name, NameCap)                          \
public:                                                                        \
  Type Name() const {                                                          \
    if (m_controller) {                                                        \
      return m_controller->Name();                                             \
    } else {                                                                   \
      return Default;                                                          \
    }                                                                          \
  }

namespace mscp {
class FormInput : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged REQUIRED)
  Q_PROPERTY(QVariant initialValue READ initialValue NOTIFY initialValueChanged)
  Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
  AUTO_PROP_REROUTE(QVariant, value, Value)
  AUTO_PROP_REROUTE_DEFAULT(bool, isDirty, IsDirty, false)
  AUTO_PROP_REROUTE(QJSValue, onValidation, OnValidation)
  AUTO_PROP_REROUTE(QString, validationError, ValidationError)
  Q_PROPERTY(mscp::FieldController *controller READ controller WRITE
                 setController NOTIFY controllerChanged)
  Q_PROPERTY(QObject *inputItem READ inputItem WRITE setInputItem NOTIFY
                 inputItemChanged)
  Q_PROPERTY(mscp::validators::Validator *cValidator READ cValidator WRITE
                 setCValidator NOTIFY cValidatorChanged)

public:
  explicit FormInput(QQuickItem *parent = nullptr);

  [[nodiscard]] QString name() const;
  void setName(const QString &value);

  [[nodiscard]] QVariant initialValue() const;
  Q_SLOT void setInitialValue();

  [[nodiscard]] bool isValid() const;

  [[nodiscard]] QObject *inputItem() const;
  void setInputItem(QObject *item);

  [[nodiscard]] FieldController *controller() const;
  void setController(FieldController *controller);

  [[nodiscard]] validators::Validator *cValidator() const;
  void setCValidator(validators::Validator *validator);

signals:
  void nameChanged();
  void inputItemChanged();
  void controllerChanged();
  void initialValueChanged();
  void isValidChanged();
  void cValidatorChanged();

private:
  QVariant m_initialValueBuffer;
  QString m_name;
  FieldController *m_controller = nullptr;
  QObject *m_inputItem = nullptr;
  bool m_isValid = true;

  void writeToInputItem(const QVariant &value);
  void setIsValid(const bool &value);
  void attemptConnectFields();
  Q_SLOT void rerouteValue();
};
} // namespace mscp
