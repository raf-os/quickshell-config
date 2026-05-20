#include "fontvalidator.h"

#include "validatorresponse.h"

#include <qcontainerfwd.h>
#include <qfontdatabase.h>
#include <qobject.h>

namespace mscp {
namespace validators {
FontValidator::FontValidator(QObject *parent) : Validator(parent) {
  m_fontFamilies = QFontDatabase::families();
};

bool FontValidator::validate(const QString &value,
                             ValidatorResponse *response) {
  if (m_fontFamilies.contains(value)) {
    if (response != nullptr) {
      *response = {
          .success = true,
          .value = value,
      };
    }
    return true;
  }

  if (response != nullptr) {
    *response = {.success = false, .message = "Font does not exist."};
  }

  return false;
}

bool FontValidator::tryValidate(const QString &value) {
  return validate(value);
}

QStringList FontValidator::fontFamilies() const { return m_fontFamilies; }
} // namespace validators
} // namespace mscp
