#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>

#include <concepts>
#include <pipewire/proxy.h>
#include <qdebug.h>
#include <qhash.h>
#include <qhashfunctions.h>
#include <qobject.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/utils/dict.h>

namespace ns::pipewire {
class Registry;

template <std::size_t N> struct TemplateString {
  char buf[N]{};

  constexpr TemplateString(const char (&str)[N]) { std::copy_n(str, N, buf); }

  constexpr operator const char *() const noexcept { return buf; }
};

struct TrackedObjectProps {
  quint32                        id;
  quint32                        perms;
  quint32                        version;
  const char                    *interface;
  const QHash<QString, QString> &initProps;
};

class TrackedObjectBase : public QObject {
  Q_OBJECT

public:
  explicit TrackedObjectBase(
      const TrackedObjectProps &props, QObject *parent = nullptr);
  ~TrackedObjectBase() override;
  Q_DISABLE_COPY_MOVE(TrackedObjectBase)

  virtual void onBeforeDestroy() {}
  void         kill();

  quint32 m_id    = 0;
  quint32 m_perms = 0;

protected:
  using string_map_t = QHash<QString, QString>;
  pw_proxy *m_object = nullptr;

  virtual void bindHooks() {}
  virtual void unbindHooks() {}

  virtual void initProps(const string_map_t &props) {}
};

template <typename T> class TrackedObject : public TrackedObjectBase {
public:
  explicit TrackedObject(const TrackedObjectProps &props,
      QObject *parent = nullptr, T *dummy = nullptr)
      : TrackedObjectBase(props, parent) {}
  ~TrackedObject() override {}
  [[nodiscard]] T *proxy() { return reinterpret_cast<T *>(m_object); }

  friend class Registry;
};

QDebug operator<<(QDebug debug, const TrackedObjectBase *object);

class TrackableObjectBase {
public:
  explicit TrackableObjectBase(quint32 id, quint32 perms);

protected:
  const quint32 m_id;
  const quint32 m_perms;
};

template <std::derived_from<TrackedObjectBase> T, TemplateString INTERFACE,
    quint32 VERSION>
class TrackableObject : public TrackableObjectBase {
public:
  explicit TrackableObject(quint32 id, quint32 perms, T *dummy = nullptr)
      : TrackableObjectBase(id, perms) {}
  ~TrackableObject() {
    if (!m_object.expired()) {
      auto sp = m_object.lock();
      sp.get()->kill();
    }
  }
  Q_DISABLE_COPY_MOVE(TrackableObject)

  static constexpr const char *staticInterfaceName() { return INTERFACE; }
  static constexpr quint32     staticVersion() { return VERSION; }

  virtual void initProps(const spa_dict *props) {
    m_initProps.clear();
    const spa_dict_item *item = nullptr;
    spa_dict_for_each(item, props) {
      m_initProps.insert(QString::fromLocal8Bit(item->key),
          QString::fromLocal8Bit(item->value));
    };
  }

  [[nodiscard]] int32_t refCount() const { return m_object.use_count(); }

  [[nodiscard]] std::shared_ptr<T> get() {
    if (m_object.expired()) {
      TrackedObjectProps props{.id = m_id,
          .perms                   = m_perms,
          .version                 = staticVersion(),
          .interface               = staticInterfaceName(),
          .initProps               = m_initProps};
      m_object = std::make_shared<T>(props);
      return m_object.lock();
    }
    return m_object.lock();
  }

protected:
  QHash<QString, QString> m_initProps;

private:
  std::weak_ptr<T> m_object;
};
} // namespace ns::pipewire
