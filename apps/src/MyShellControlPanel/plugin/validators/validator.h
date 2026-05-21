#pragma once

#include "validatorresponse.h"
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace mscp {
namespace validators {
class Validator : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

public:
  explicit Validator(QObject *parent = nullptr) : QObject(parent) {}
  virtual bool validate(const QVariant &value,
                        ValidatorResponse *response = nullptr) = 0;
  virtual ~Validator() = default;
};
} // namespace validators
} // namespace mscp
