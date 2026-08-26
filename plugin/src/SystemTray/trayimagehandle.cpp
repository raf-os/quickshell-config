#include "trayimagehandle.h"

#include "dbusimage.h"
#include "statusnotifieritem.h"

namespace ns::systemtray {
TrayImageHandle::TrayImageHandle(StatusNotifierItem *item)
    : dbusprovider::BaseImageHandle(), m_item(item) {}
} // namespace ns::systemtray
