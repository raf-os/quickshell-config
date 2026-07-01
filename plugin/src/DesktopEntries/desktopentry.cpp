#include "desktopentry.h"

#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <utility>

namespace ns::desktop::entries {
DesktopEntry::DesktopEntry(QString  id,
                           QObject *parent)
    : QObject(parent),
      m_id(std::move(id)) {}

void DesktopEntry::updateState(const EntryData &newState) {
  {
    QScopedPropertyUpdateGroup group;

    m_name = newState.name;
    m_icon = newState.icon;
  }

  m_state = newState;
  this->updateActions(newState.actions);
}

void DesktopEntry::updateActions(const QList<EntryActionData> &newActions) {}
} // namespace ns::desktop::entries
