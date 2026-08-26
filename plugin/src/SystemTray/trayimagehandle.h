#pragma once

#include "dbusimage.h"
#include "statusnotifieritem.h"
namespace ns::systemtray {
class TrayImageHandle : public dbusprovider::BaseImageHandle {
public:
  explicit TrayImageHandle(StatusNotifierItem *item);

private:
  StatusNotifierItem *m_item;
};
} // namespace ns::systemtray
