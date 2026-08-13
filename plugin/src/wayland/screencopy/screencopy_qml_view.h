#pragma once

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qquickitem.h>
#include <qsgnode.h>
#include <qsize.h>
#include <qtmetamacros.h>

#include "screencopy_context.h"

namespace ns::wayland::screencopy {
class ScreencopyQMLView : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QObject *captureSource READ captureSource WRITE setCaptureSource
                 NOTIFY captureSourceChanged)
  Q_PROPERTY(bool isLive READ isLive WRITE setIsLive NOTIFY isLiveChanged)
  Q_PROPERTY(bool hasContent READ default NOTIFY hasContentChanged BINDABLE
                 bindableHasContent)
  Q_PROPERTY(QSize sourceSize READ default NOTIFY sourceSizeChanged BINDABLE
                 bindableSourceSize)
  Q_PROPERTY(QSizeF constraints READ default NOTIFY constraintsChanged BINDABLE
                 bindableConstraints)

public:
  explicit ScreencopyQMLView(QQuickItem *parent = nullptr);

  void componentComplete() override;

  [[nodiscard]] bool isLive() const;
  void               setIsLive(bool value);

  [[nodiscard]] QObject *captureSource() const;
  void                   setCaptureSource(QObject *source);

  [[nodiscard]] QBindable<bool>   bindableHasContent() { return &b_hasContent; }
  [[nodiscard]] QBindable<QSize>  bindableSourceSize() { return &b_sourceSize; }
  [[nodiscard]] QBindable<QSizeF> bindableConstraints() {
    return &b_constraints;
  }

  Q_INVOKABLE void captureSingleFrame();

signals:
  void stopped();

  void captureSourceChanged();
  void isLiveChanged();
  void hasContentChanged();
  void sourceSizeChanged();
  void constraintsChanged();

protected:
  QSGNode *updatePaintNode(QSGNode             *oldNode,
                           UpdatePaintNodeData *data) override;

private slots:
  void onContextStopped();
  void onFrameCaptured();
  void onCaptureSourceDestroyed();
  void onBuffersReady();

  void _onImplicitSizeChanged();

private:
  void createContext();
  void deleteContext(bool shouldUpdate = true);
  void captureFrame();

  // This function assumes a context already exists. Otherwise, a segfault
  // happens.
  inline void captureFrameUnsafe() { m_context->captureFrame(); }

  Q_OBJECT_BINDABLE_PROPERTY(ns::wayland::screencopy::ScreencopyQMLView,
                             bool,
                             b_hasContent,
                             &ScreencopyQMLView::hasContentChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::wayland::screencopy::ScreencopyQMLView,
                             QSize,
                             b_sourceSize,
                             &ScreencopyQMLView::sourceSizeChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::wayland::screencopy::ScreencopyQMLView,
                             QSizeF,
                             b_constraints,
                             &ScreencopyQMLView::constraintsChanged)
  Q_OBJECT_BINDABLE_PROPERTY(ns::wayland::screencopy::ScreencopyQMLView,
                             QSizeF,
                             _implicitSize,
                             &ScreencopyQMLView::_onImplicitSizeChanged)

  bool m_isLive               = false;
  bool m_isCompleted          = false;
  bool m_queuedContextCapture = false;

  QObject           *m_captureSource = nullptr;
  ScreencopyContext *m_context       = nullptr;
};
} // namespace ns::wayland::screencopy
