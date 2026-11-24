#pragma once

#include <qobjectdefs.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "ns_layershell_export.h"

namespace ns::layershell {
namespace LayerAnchor {
Q_NAMESPACE_EXPORT(NS_LAYERSHELL_EXPORT)

enum Anchor {
  None   = 0,
  Top    = 1 << 0,
  Bottom = 1 << 1,
  Left   = 1 << 2,
  Right  = 1 << 3
};

Q_DECLARE_FLAGS(Anchors, Anchor)
Q_FLAG_NS(Anchors)
}; // namespace LayerAnchor

namespace WlrLayer {
Q_NAMESPACE_EXPORT(NS_LAYERSHELL_EXPORT)

enum Enum { Background = 0, Bottom = 1, Top = 2, Overlay = 3 };

Q_ENUM_NS(Enum)
}; // namespace WlrLayer

namespace KeyboardInteractivity {
Q_NAMESPACE_EXPORT(NS_LAYERSHELL_EXPORT)

enum Enum { None = 0, Exclusive = 1, OnDemand = 2 };

Q_ENUM_NS(Enum)
} // namespace KeyboardInteractivity
} // namespace ns::layershell
