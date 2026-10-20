#pragma once

#include <algorithm>
#include <cstddef>

#include <pipewire/proxy.h>
#include <qdebug.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <spa/utils/dict.h>

namespace ns::services::pipewire {
class PwRegistry;
class PwBindableBase;

template <std::size_t N> struct TemplateString {
  char buf[N]{};

  constexpr TemplateString(const char (&str)[N]) { std::copy_n(str, N, buf); }

  constexpr operator const char *() const noexcept { return buf; }
};

struct PwBindableProps {
  PwRegistry *registry;
  quint32     id;
  quint32     perms;
};

class IPwBindableAttached {
public:
  virtual constexpr const char *interface_name() const = 0;
  virtual constexpr quint32     version() const        = 0;
};

class PwBindableBase : public QObject {
  Q_OBJECT

public:
  explicit PwBindableBase(PwBindableProps props);
  ~PwBindableBase() override;

  virtual void initProps(const spa_dict * /* props */) {}
  virtual void kill();

  quint32 m_id    = 0;
  quint32 m_perms = 0;

  pw_proxy *proxy();

protected:
  virtual void bindHooks()   = 0;
  virtual void unbindHooks() = 0;

  virtual void bind();
  void         unbind();

  void bindRegistry(const char *interface, quint32 version);

  pw_proxy   *m_proxy    = nullptr;
  PwRegistry *m_registry = nullptr;
};

QDebug operator<<(QDebug debug, const PwBindableBase *object);

template <typename T, TemplateString INTERFACE, quint32 VERSION>
class PwBindableInterface : public IPwBindableAttached {
public:
  constexpr const char *interface_name() const override {
    constexpr auto *c = static_cast<const char *>(INTERFACE);
    return c;
  }
  constexpr quint32 version() const override { return VERSION; }

  explicit PwBindableInterface(PwBindableBase *parent) : m_parent(parent) {}

  T *proxy() { return reinterpret_cast<T *>(m_parent->proxy()); }

private:
  PwBindableBase *m_parent;

  friend class PwRegistry;
};
} // namespace ns::services::pipewire
