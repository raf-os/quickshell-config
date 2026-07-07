#include "entryaction.h"
#include "desktopentry.h"
#include "entrymanager.h"

#include <qobject.h>
#include <utility>

namespace ns::desktop::entries {
EntryAction::EntryAction(QString       id,
                         DesktopEntry *entry)
    : QObject(entry),
      m_entry(entry),
      m_id(std::move(id)) {}

QString EntryAction::getId() const { return m_id; }

DesktopEntry *EntryAction::entry() { return m_entry; }

void EntryAction::execute() {
  EntryManager::instance()->executeGeneric(
      m_command, m_entry->bindableWorkingDirectory().value(), m_entry);
}
} // namespace ns::desktop::entries
