#pragma once

#include <qjsengine.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpointer.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qvectornd.h>

namespace ns::components {
class SToolTip;

namespace TooltipAnchor {
Q_NAMESPACE
QML_NAMED_ELEMENT(TooltipAnchor)

// Do not reorganize
enum Enum { Right = 0, Bottom, Left, Top };

Q_ENUM_NS(Enum)
} // namespace TooltipAnchor

namespace TooltipAlignment {
Q_NAMESPACE
QML_NAMED_ELEMENT(TooltipAlignment)

enum Enum { Start, Middle, End };

Q_ENUM_NS(Enum)
} // namespace TooltipAlignment

class TooltipProvider : public QObject {
  Q_OBJECT
  QML_ANONYMOUS
  QML_SINGLETON

public:
  [[nodiscard]] bool isActive() const;

  static TooltipProvider *instance(QQmlEngine *engine);
  static TooltipProvider *create(QQmlEngine *engine, QJSEngine *);

  void requestOwnership(SToolTip *owner);
  void releaseOwnership(SToolTip *owner);

public slots:
  void updatePosition();

private slots:
  void onTooltipDestroyed();
  void onOwnerTextChanged();
  void onOwnerDestroyed();
  void onGraceTimerTimeout();
  void onOwnerWindowChanged(QQuickWindow *window);
  void refit();

private:
  explicit TooltipProvider(QQmlEngine *engine, QObject *parent = nullptr);
  ~TooltipProvider() override;

  QTimer                *m_graceTimer;
  QQuickWindow          *m_window;
  QQuickItem            *m_windowContent   = nullptr;
  QQuickItem            *m_tooltip         = nullptr;
  SToolTip              *m_owner           = nullptr;
  QPointer<QQuickWindow> m_referenceWindow = nullptr;
  QVector2D              m_mappedPosition;

  void mapPosition();
  void clearOwnerSignals();

  QQuickItem *instantiateTooltip(QQmlEngine *engine = nullptr);
};
} // namespace ns::components
