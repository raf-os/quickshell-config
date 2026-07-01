#include "localeutil.h"

namespace ns::desktop::entries {
int LocaleUtils::matchScore(const LocaleUtils &other) const {
  if (this->language != other.language)
    return 0;

  if (!other.modifier.isEmpty() && this->modifier != other.modifier)
    return 0;
  if (!other.territory.isEmpty() && this->territory != other.territory)
    return 0;

  auto score = 1;

  if (!other.territory.isEmpty())
    score += 2;
  if (!other.modifier.isEmpty())
    score += 1;

  return score;
}
} // namespace ns::desktop::entries
