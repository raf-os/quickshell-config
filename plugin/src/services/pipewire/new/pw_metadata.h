#pragma once

#include <cstdint>

#include <pipewire/extensions/metadata.h>
#include <qhashfunctions.h>
#include <qtmetamacros.h>

#include "pw_spa_hook.h"
#include "pw_trackable.h"

namespace ns::pipewire {
class Metadata : public TrackedObject<pw_metadata> {
  Q_OBJECT

public:
  void bindHooks() override;
  void unbindHooks() override;
  void initProps(const string_map_t &props) override;

  [[nodiscard]] const QString &name() const;
  [[nodiscard]] bool           hasSetPermissions() const;

  void setProperty(const char *key, const char *type, const char *value);

signals:
  void propertyChanged(const char *key, const char *type, const char *value);

private:
  SpaHook m_hook;
  QString m_name;

  static const struct pw_metadata_events METADATA_EVENTS;
  static int on_property(void *data, uint32_t subject, const char *key,
      const char *type, const char *value);
};

using TrackableMetadata =
    TrackableObject<Metadata, PW_TYPE_INTERFACE_Metadata, PW_VERSION_METADATA>;
} // namespace ns::pipewire
