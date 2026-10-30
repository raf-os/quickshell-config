#pragma once

#include <cstdint>

#include <pipewire/extensions/metadata.h>
#include <qhashfunctions.h>
#include <qtmetamacros.h>
#include <spa/utils/dict.h>

#include "pw_spa_hook.h"
#include "pw_trackable.h"

namespace ns::pipewire {
class Metadata : public TrackedObject<pw_metadata> {
  Q_OBJECT

public:
  explicit Metadata(const TrackedObjectProps &props, QObject *parent = nullptr)
      : TrackedObject(props, nullptr) {}
  ~Metadata() override {}

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

class TrackableMetadata : public TrackableObject<Metadata,
                              PW_TYPE_INTERFACE_Metadata, PW_VERSION_METADATA> {
public:
  explicit TrackableMetadata(quint32 id, quint32 perms)
      : TrackableObject(id, perms) {}

  [[nodiscard]] QString name() const;

  void initProps(const spa_dict *props) override;

private:
  QString m_name;
};

} // namespace ns::pipewire
