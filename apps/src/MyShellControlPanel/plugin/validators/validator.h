#pragma once

#include "validatorresponse.h"
#include <qobject.h>
#include <qtmetamacros.h>

namespace mscp {
namespace validators {
class Validator : public QObject {
  Q_OBJECT
protected:
  explicit Validator(QObject *parent = nullptr) : QObject(parent) {}

public:
  virtual bool validate(const QVariant &value,
                        ValidatorResponse *response = nullptr) = 0;
  virtual ~Validator() = default;
};
} // namespace validators
} // namespace mscp
