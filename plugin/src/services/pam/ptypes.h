#pragma once

#include <cstdint>

#include <qobjectdefs.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace ns::services::pam {
enum class PamEvent : uint8_t { Request, Exit };

enum class PamExitCode : uint8_t {
  Success,
  StartFailed,
  AuthFailed,
  MaxTries,
  PamError,
  OtherError,
};

namespace PamResult {
Q_NAMESPACE
QML_NAMED_ELEMENT(PamResult)

enum Enum : quint8 { Success = 0, Failed = 1, Error = 2, MaxTries = 3 };

Q_ENUM_NS(Enum)
}; // namespace PamResult

namespace PamError {
Q_NAMESPACE
QML_NAMED_ELEMENT(PamError)

enum Enum : quint8 { StartFailed = 1, TryAuthFailed = 2, InternalError = 3 };

Q_ENUM_NS(Enum)
} // namespace PamError
} // namespace ns::services::pam
