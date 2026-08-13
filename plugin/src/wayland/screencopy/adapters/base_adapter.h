#pragma once

#include <qobject.h>

namespace ns::wayland::screencopy {
class BaseAdapter : public QObject {
protected:
  explicit BaseAdapter(QObject *parent = nullptr) : QObject(parent) {}
};
} // namespace ns::wayland::screencopy
