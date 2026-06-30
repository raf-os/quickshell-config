#pragma once

#include <qdir.h>
#include <qtenvironmentvariables.h>
namespace ns::desktopentries {
// Desktop Entry Specification - Localized values for keys
// https://specifications.freedesktop.org/desktop-entry/latest/localized-keys.html
//
// Adapted from quickshell
// https://git.outfoxxed.me/quickshell/quickshell/src/branch/master/src/core/desktopentry.cpp
class LocaleUtils {
public:
  explicit LocaleUtils() = default;

  explicit LocaleUtils(const QString &str) {
    auto territoryIdx = str.indexOf('_');
    auto codesetIdx   = str.indexOf('.');
    auto modifierIdx  = str.indexOf('@');

    auto parseEnd = str.length();

    if (modifierIdx != -1) {
      this->modifier = str.sliced(modifierIdx + 1, parseEnd - modifierIdx - 1);
      parseEnd       = modifierIdx;
    }

    if (codesetIdx != -1) {
      parseEnd = codesetIdx;
    }

    if (territoryIdx != -1) {
      this->territory =
          str.sliced(territoryIdx + 1, parseEnd - territoryIdx - 1);
      parseEnd = territoryIdx;
    }

    this->language = str.sliced(0, parseEnd);
  }

  [[nodiscard]] bool isValid() const { return !this->language.isEmpty(); }

  [[nodiscard]] int matchScore(const LocaleUtils &other) const;

  static const LocaleUtils &systemLocale() {
    static LocaleUtils *locale = nullptr;

    if (locale == nullptr) {
      auto l = qEnvironmentVariable("LC_MESSAGES");
      if (l.isEmpty())
        l = qEnvironmentVariable("LANG");
      locale = new LocaleUtils(l);
    }

    return *locale;
  }

  QString language;
  QString territory;
  QString modifier;
};
} // namespace ns::desktopentries
