#pragma once

#include <QtPlugin>
#include <qobject.h>

namespace myqmlplugin {
namespace configs {
class IConfigSerializer {
public:
  virtual void commitSave() = 0;

  virtual ~IConfigSerializer() {}
};
} // namespace configs
} // namespace myqmlplugin

Q_DECLARE_INTERFACE(myqmlplugin::configs::IConfigSerializer,
                    "com.myshell.IConfigSerializer/1.0")
