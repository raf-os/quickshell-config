#include "desktopentry.h"
#include "entryaction.h"
#include "entrymanager.h"

#include <algorithm>
#include <qlist.h>
#include <qobject.h>
#include <qproperty.h>
#include <utility>

namespace ns::desktop::entries {
DesktopEntry::DesktopEntry(QString  id,
                           QObject *parent)
    : QObject(parent),
      m_id(std::move(id)) {}

bool DesktopEntry::isValid() { return m_name.value().isEmpty(); }

QString DesktopEntry::id() const { return m_id; }

void DesktopEntry::updateState(const EntryData &newState) {
  {
    QScopedPropertyUpdateGroup group;

    m_name             = newState.name;
    m_genericName      = newState.genericName;
    m_startupClass     = newState.startupClass;
    m_icon             = newState.icon;
    m_comment          = newState.comment;
    m_execString       = newState.execStr;
    m_workingDirectory = newState.workingDirectory;
    m_command          = newState.command;
    m_categories       = newState.categories;
    m_keywords         = newState.keywords;
    m_noDisplay        = newState.noDisplay;
    m_hidden           = newState.hidden;
  }

  m_state = newState;
  this->updateActions(newState.actions);
}

void DesktopEntry::updateActions(const QList<EntryActionData> &newActions) {
  auto old = m_actions;
  m_actions.clear();

  for (const auto &a : newActions) {
    EntryAction *_action = nullptr;
    auto         it      = std::ranges::find(old, a.id, &EntryAction::m_id);
    if (it != old.end()) {
      // preserve old items
      _action = *it;
      old.erase(it);
    } else {
      _action = new EntryAction(a.id, this);
    }

    {
      QScopedPropertyUpdateGroup group;
      _action->m_name    = a.name;
      _action->m_icon    = a.icon;
      _action->m_command = a.command;
    }

    m_actions.append(_action);
  }

  for (auto *cleanup : old) {
    cleanup->deleteLater();
  }
}

void DesktopEntry::incrementFrequency() {
  this->bindableFrequency().setValue(this->bindableFrequency().value() + 1);
}

void DesktopEntry::execute() {
  EntryManager::instance()->executeGeneric(m_command, m_workingDirectory, this);
}
} // namespace ns::desktop::entries
