#pragma once

#include <qobject.h>
#include <qvariant.h>
namespace mscp {
namespace validators {
struct ValidatorResponse {
  bool success;
  QVariant value;
  QString message;
};
} // namespace validators
} // namespace mscp
