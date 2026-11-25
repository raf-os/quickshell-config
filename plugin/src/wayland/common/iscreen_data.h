#pragma once

#include <QByteArrayList>
#include <QtPlugin>
#include <QtTypes>

namespace ns {
class IScreenData {
public:
  virtual ~IScreenData() = default;

  [[nodiscard]] virtual int      propCount() const          = 0;
  [[nodiscard]] virtual QVariant varAt(qsizetype idx) const = 0;
};
} // namespace ns

Q_DECLARE_INTERFACE(ns::IScreenData, "org.nightshell.Wayland.IScreenData")
