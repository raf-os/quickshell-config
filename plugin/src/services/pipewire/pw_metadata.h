#pragma once

#include <pipewire/extensions/metadata.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "pw_bindable.h"
#include "pw_spa_hook.h"

namespace ns::services::pipewire {
class PwMetadata : public PwBindableBase {
  Q_OBJECT

public:
  explicit PwMetadata(PwBindableProps props);

  void bindHooks() override;
  void unbindHooks() override;

  [[nodiscard]] const QString &name() const;
  [[nodiscard]] bool           hasSetPermissions() const;

  void setProperty(const char *key, const char *type, const char *value);

signals:
  void propertyChanged(QString key, QString type, QString value);

private:
  SpaHook m_listener;
  PwBindableInterface<pw_metadata, PW_TYPE_INTERFACE_Metadata,
      PW_VERSION_METADATA>
      m_interface{this};

  QString m_name;
};
} // namespace ns::services::pipewire
