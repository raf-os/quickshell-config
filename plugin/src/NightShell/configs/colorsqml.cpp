#include "colorsqml.h"

#include <qcolor.h>
#include <qobject.h>

#include "colors.h"

namespace ns::configs {
ColorsQML::ColorsQML(QObject *parent) : QObject(parent) { setupConnections(); }

void ColorsQML::onCurrentThemeChanged() { setupConnections(); }

void ColorsQML::setupConnections() {
  auto curTheme = Colors::instance()->current();

  auto themeMeta = curTheme->metaObject();
  auto thisMeta  = this->metaObject();

  Qt::beginPropertyUpdateGroup();
  for (auto i = thisMeta->propertyOffset(); i < thisMeta->propertyCount(); i++)
  {
    auto p = thisMeta->property(i);

    if (!p.isBindable()) continue;

    auto bindable = p.bindable(this);
    bindable.takeBinding();

    auto tid = themeMeta->indexOfProperty(p.name());

    if (tid == -1) {
      if (p.isResettable()) {
        p.reset(this);
      } else {
        p.bindable(this).takeBinding();
      }
      continue;
    }

    auto tp = themeMeta->property(tid);
    if (tp.metaType() != p.metaType()) continue;

    if (!tp.isBindable()) continue;

    bindable.setBinding(tp.bindable(curTheme).makeBinding());
  }
  Qt::endPropertyUpdateGroup();
}
} // namespace ns::configs
