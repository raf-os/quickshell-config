#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include <concepts>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "ns_pipewire_debug.h"
#include "pw_trackable.h"

namespace ns::pipewire {
class PwObjectInterface : public QObject {
  Q_OBJECT

public:
  explicit PwObjectInterface(
      TrackedObjectBase *object, QObject *parent = nullptr);
  virtual ~PwObjectInterface() override;

  virtual void                     init() {}
  [[nodiscard]] TrackedObjectBase *trackedObject();

signals:
  void destroying();

private slots:
  void onTrackedDestroyed();

protected:
  TrackedObjectBase *m_tracked;
  virtual void       cleanup() {}

  quint32 m_id = 0;
};

template <typename T>
  requires std::derived_from<T, TrackedObjectBase>
class PwTrackedQml : public PwObjectInterface {
  using type_t = std::enable_if_t<std::derived_from<T, TrackedObjectBase>,
      TrackedObjectBase>;

protected:
  using shared_ptr_t = std::shared_ptr<T>;

public:
  explicit PwTrackedQml(shared_ptr_t object, QObject *parent = nullptr)
      : PwObjectInterface(object.get(), parent), m_object(object) {}
  virtual ~PwTrackedQml() override = default;

  [[nodiscard]] T *object() { return m_object.get(); }
  [[nodiscard]] T *object() const { return m_object.get(); }

  void cleanup() override { m_object.reset(); }

private:
  shared_ptr_t m_object;
};
} // namespace ns::pipewire
