#include "qml_mpris_singleton.h"

#include <qobject.h>

#include "helpermacros.h"
#include "mpris_service_watcher.h"

namespace ns::mpris {
AUTO_MEYERS_SINGLETON_QML_IMPL(MprisSingletonQML)

MprisSingletonQML::MprisSingletonQML(QObject *parent)
    : QObject(parent), m_watcher(new MprisServiceWatcher(this)) {}

bool MprisSingletonQML::isValid() const { return m_watcher->isValid(); }

MprisPlayerListModel *MprisSingletonQML::model() { return m_watcher->model(); }
} // namespace ns::mpris
