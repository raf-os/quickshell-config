#pragma once

#include <memory>
#include <type_traits>

#include <concepts>
#include <qobject.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

#include "pw_trackable.h"

namespace ns::pipewire {
class PwObjectInterface : public QObject {
  Q_OBJECT

public:
  explicit PwObjectInterface(
      TrackedObjectBase *object, QObject *parent = nullptr);
  ~PwObjectInterface() override = default;
  Q_DISABLE_COPY_MOVE(PwObjectInterface)

  virtual void                     init() {}
  [[nodiscard]] TrackedObjectBase *trackedObject();

signals:
  void destroying();

private slots:
  void onTrackedDestroyed();

protected:
  TrackedObjectBase *m_tracked;
};

template <typename T>
  requires std::derived_from<T, TrackedObjectBase>
class PwTrackedQml : public PwObjectInterface {
  using type_t = std::enable_if_t<std::derived_from<T, TrackedObjectBase>,
      TrackedObjectBase>;

protected:
  using shared_ptr_t = std::shared_ptr<T>;

public:
  explicit PwTrackedQml(
      std::shared_ptr<type_t> object, QObject *parent = nullptr)
      : PwObjectInterface(object.get(), parent) {
    init();
  }
  ~PwTrackedQml() override = default;

  [[nodiscard]] T *object() { return m_object.get(); }
  [[nodiscard]] T *object() const { return m_object.get(); }

private:
  shared_ptr_t m_object;
};
} // namespace ns::pipewire
