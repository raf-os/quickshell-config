#include "systemsounds.h"

#include <cstdint>

#include <canberra.h>
#include <qdebug.h>
#include <qlist.h>
#include <qlogging.h>
#include <qobject.h>
#include <qstringview.h>

#include "config.h"
#include "hyprevents.h"

namespace ns {
SystemSounds::SystemSounds(QObject *parent) : QObject(parent) {
  if (auto error = ca_context_create(&m_ctx); error < 0) {
    qWarning() << "ns::SystemSounds: Error creating canberra context: "
               << ca_strerror(error);
    deleteLater();
    return;
  }

  ca_proplist *plist = nullptr;

  if (auto error = ca_proplist_create(&plist); error < 0) {
    qWarning() << "ns::SystemSounds: Error creating canberra proplist: "
               << ca_strerror(error);
    deleteLater();
    return;
  }

  auto       t = myqmlplugin::configs::Config::instance()->sounds()->theme();
  QByteArray themeName = t.toUtf8();

  ca_proplist_sets(plist, "application.name", "nightshell");
  ca_proplist_sets(plist, "application.id", "org.nightshell");
  ca_proplist_sets(plist, "canberra.xdg-theme.name", themeName);

  ca_context_change_props_full(m_ctx, plist);
  ca_proplist_destroy(plist);

  bool                    success = false;
  const QList<QByteArray> drivers{"pulse", "alsa"};
  for (const auto &driver : drivers) {
    ca_context_set_driver(m_ctx, driver);
    auto result = ca_context_open(m_ctx);
    if (result == 0) {
      success = true;
      break;
    }
    qWarning() << "ns::SystemSounds: Failed setting context driver '"
               << ca_strerror(result) << "'.";
  }

  if (!success) {
    qWarning() << "ns::SystemSounds: Failed setting drivers for this context.";
    deleteLater();
    return;
  }
}

SystemSounds::~SystemSounds() {
  if (m_ctx) ca_context_destroy(m_ctx);
}

void SystemSounds::onSoundFinished(ca_context *c,
                                   uint32_t    id,
                                   int         error,
                                   void       *userData) {
  if (userData) {
    auto data                      = static_cast<CallbackProps *>(userData);
    auto objRef                    = data->ref;
    objRef->ActiveSounds.BellSound = false;

    delete data;
  }
}

ns::hyprland::HyprEvents *SystemSounds::eventHandler() const {
  return m_eventHandler;
}
void SystemSounds::setEventHandler(ns::hyprland::HyprEvents *value) {
  if (m_eventHandler == value) return;

  if (m_eventHandler) {
    QObject::disconnect(this, nullptr, m_eventHandler, nullptr);
  }

  m_eventHandler = value;
  emit eventHandlerChanged();

  if (m_eventHandler) {
    QObject::connect(m_eventHandler, &QObject::destroyed, this, [this]() {
      setEventHandler(nullptr);
    });
    QObject::connect(m_eventHandler,
                     &ns::hyprland::HyprEvents::bellRang,
                     this,
                     &SystemSounds::onBellRequested);
  }
}

bool SystemSounds::onBellRequested() {
  if (!m_ctx) return false;
  if (ActiveSounds.BellSound) return false;

  auto udata = new CallbackProps{this};

  ca_proplist *plist = nullptr;
  if (auto error = ca_proplist_create(&plist); error != CA_SUCCESS) {
    qWarning() << "ns::SystemSounds::onBellRequested: Failed creating canberra "
                  "proplist: "
               << ca_strerror(error);
    delete udata;
    return false;
  }

  ca_proplist_sets(plist, "event.id", "bell");
  ca_proplist_sets(plist,
                   "event.description",
                   "An application has requested the bell sound.");

  auto result = ca_context_play_full(m_ctx,
                                     ContextId::BellSound,
                                     plist,
                                     &SystemSounds::onSoundFinished,
                                     udata);
  ca_proplist_destroy(plist);

  if (result == CA_SUCCESS) {
    ActiveSounds.BellSound = true;
    return true;
  }

  delete udata; // failed playing, we need to manually deallocate this
  qWarning() << "ns::SystemSounds::onBellRequested: Failed playing sound: "
             << ca_strerror(result);
  return false;
}
} // namespace ns
