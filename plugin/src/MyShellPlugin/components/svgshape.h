#pragma once

// TODO: Look into QtQuick's PathSvg
// Manually parse SVG and create one of the above for every <path> node
// Hardware accelerated SVG rendering, maybe?

#include <qhash.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qstringview.h>
#include <qtmetamacros.h>

namespace ns {
namespace components {
class SVGCache : QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit SVGCache(QObject *parent = nullptr);

private:
  QHash<QString, QByteArray> m_entries;
};
} // namespace components
} // namespace ns
