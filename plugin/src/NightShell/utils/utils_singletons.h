#pragma once

#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

#include "keyboard_layout_serializer.h"
#include "ns_utils_export.h"

namespace ns::utils {
class NS_UTILS_EXPORT UtilsSingleton : public QObject {
  Q_OBJECT
  QML_ANONYMOUS

public:
  Q_DISABLE_COPY_MOVE(UtilsSingleton)

  static UtilsSingleton *instance();
  static UtilsSingleton *create(QQmlEngine *engine, QJSEngine * /*unused*/);

  static KeyboardLayoutXMLSerializer *keyboardLayoutSerializer();

private:
  explicit UtilsSingleton(QObject *parent = nullptr);
  ~UtilsSingleton() override;

  KeyboardLayoutXMLSerializer *m_kbdLayoutSerializer = nullptr;
};
} // namespace ns::utils
