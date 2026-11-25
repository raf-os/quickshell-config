#pragma once

#include <QtPlugin>

namespace ns {
class IWorkspaceData {
public:
  virtual ~IWorkspaceData() = 0;

  [[nodiscard]] virtual int      propCount() const          = 0;
  [[nodiscard]] virtual QVariant varAt(qsizetype idx) const = 0;
};
} // namespace ns

Q_DECLARE_INTERFACE(ns::IWorkspaceData, "org.nightshell.Wayland.IWorkspaceData")
