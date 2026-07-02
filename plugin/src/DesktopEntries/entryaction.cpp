#include "entryaction.h"
#include "desktopentry.h"

#include <qobject.h>
#include <utility>

namespace ns::desktop::entries {
EntryAction::EntryAction(QString       id,
                         DesktopEntry *entry)
    : QObject(entry),
      m_entry(entry),
      m_id(std::move(id)) {}

QString EntryAction::getId() const { return m_id; }
} // namespace ns::desktop::entries
