#pragma once

#include "validator.h"
#include "validatorresponse.h"

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qvariant.h>

namespace mscp {
namespace validators {
class FontValidator : public Validator {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QStringList fontFamilies READ fontFamilies CONSTANT)

public:
  explicit FontValidator(QObject *parent = nullptr);

  bool validate(const QVariant &value,
                ValidatorResponse *response = nullptr) override;
  Q_INVOKABLE bool tryValidate(const QString &value);
  [[nodiscard]] QStringList fontFamilies() const;

private:
  QStringList m_fontFamilies;
};
} // namespace validators
} // namespace mscp
