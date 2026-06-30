#include "desktopentry.h"
#include "entrymanager.h"

#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>

namespace ns::desktopentries {
DesktopEntry::DesktopEntry(QObject *parent) : QObject(parent) {}

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
} // namespace ns::desktopentries
