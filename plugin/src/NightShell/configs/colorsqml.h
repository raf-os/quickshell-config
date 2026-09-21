#pragma once

#include <qcolor.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "helpermacros.h"

namespace ns::configs {
class ColorsQML : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Colors)
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML(ColorsQML)

#define C(Name, Value) AUTO_BINDABLE_DEFAULT(ColorsQML, QColor, Name, Value)
#include "colors.def"
#undef C

private slots:
  void onCurrentThemeChanged();

private:
  explicit ColorsQML(QObject *parent = nullptr);

  void setupConnections();
};
} // namespace ns::configs
