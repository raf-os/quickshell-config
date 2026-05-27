#include "svgicon.h"

#include <QDomDocument>
#include <QDomElement>
#include <qbrush.h>
#include <qcolor.h>
#include <qdir.h>
#include <qdom.h>
#include <qfileinfo.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qquickitem.h>
#include <qquickpainteditem.h>
#include <qsize.h>
#include <qstringview.h>
#include <qsvgrenderer.h>

namespace ns {
namespace components {
SVGIcon::SVGIcon(QQuickItem *parent)
    : QQuickPaintedItem(parent), m_color(QColor("white")),
      m_renderer(new QSvgRenderer(this)) {
  m_renderer->setAspectRatioMode(Qt::KeepAspectRatio);

  QObject::connect(m_renderer, &QSvgRenderer::repaintNeeded, this,
                   &SVGIcon::triggerUpdate);
}

void SVGIcon::triggerUpdate() { update(); }

void SVGIcon::paint(QPainter *painter) {
  painter->setBackgroundMode(Qt::TransparentMode);
  painter->setPen(Qt::NoPen);

  if (width() == 0 || height() == 0)
    return;

  if (!m_renderer->isValid()) {
    QBrush brush(QColor("#e70dff"));
    painter->setBrush(brush);
    painter->drawRect(0, 0, width(), height());
    return;
  }

  const QSize desiredSize = QSize(width(), height());
  QSize resolvedSize =
      desiredSize.isValid() ? std::move(desiredSize) : QSize(0, 0);

  QRectF bounds(0, 0, m_size, m_size);
  // WARNING: THIS IS RASTERIZING THE SVG ON THE CPU!
  // THAT MEANS ANIMATIONS ARE NOT OPTIMIZED!
  m_renderer->render(painter, bounds);
}

QString SVGIcon::iconName() const { return m_iconName; }

void SVGIcon::setIconName(const QString &name) {
  if (name == m_iconName)
    return;

  m_iconName = name;
  processXmlData(m_iconName);
  emit iconNameChanged();
}

QColor SVGIcon::color() const { return m_color; }
void SVGIcon::setColor(const QColor &value) {
  if (m_color == value)
    return;

  m_color = value;
  emit colorChanged();
}

qreal SVGIcon::size() const { return m_size; }
void SVGIcon::setSize(const qreal &value) {
  if (m_size == value)
    return;

  m_size = value;
  setWidth(m_size);
  setHeight(m_size);

  emit sizeChanged();
}

void SVGIcon::processXmlData(const QString &source) {
  if (source.isEmpty())
    return;

  const QString allowedPath = QDir(m_iconDir).canonicalPath();
  const QString iconPath = m_iconDir + "/" + source + ".svg";

  QFile iconFile(iconPath);

  if (!iconFile.exists()) {
    qWarning() << "ns::components::processXmlData: Provided icon '" << source
               << "' not found.";
    return;
  }

  const QString canonical = QFileInfo(iconPath).canonicalPath();

  if (!canonical.startsWith(allowedPath)) {
    qWarning() << "ns::components::processXmlData: Attempted to fetch icon "
                  "outside of allowed path!";
    return;
  }

  if (!iconFile.open(QIODevice::ReadOnly)) {
    qWarning() << "ns::components::processXmlData: Unable to open file "
               << iconPath << " for reading.";
    return;
  }

  QByteArray buffer(iconFile.size(), Qt::Uninitialized);
  iconFile.read(buffer.data(), buffer.size());

  QDomDocument doc;
  doc.setContent(buffer);

  auto rootElement = doc.documentElement();

  if (rootElement.tagName() != "svg") {
    qWarning()
        << "ns::components::processXmlData: Invalid svg document provided.";
    return;
  }

  rootElement.setAttribute("fill", m_color.name(QColor::HexRgb));

  m_renderer->load(doc.toByteArray(0));

  update();
}
} // namespace components
} // namespace ns
