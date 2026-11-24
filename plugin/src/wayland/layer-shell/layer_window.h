#pragma once

#include <memory>

#include <QObject>
#include <QProperty>
#include <QScreen>
#include <QWindow>

#include "layer_enums.h"
#include "ns_layershell_export.h"

namespace ns::layershell {
class LayerWindowPrivate;

class NS_LAYERSHELL_EXPORT LayerWindow : public QObject {
  Q_OBJECT

  Q_PROPERTY(QScreen *screen READ screen WRITE setScreen NOTIFY screenChanged)

#define NS_LW_B(Type, Name, InitialValue)                                      \
  Q_PROPERTY(                                                                  \
      Type Name READ default WRITE default NOTIFY Name##Changed BINDABLE       \
                                                  bindable_##Name)             \
public:                                                                        \
  [[nodiscard]] QBindable<Type> bindable_##Name() { return &b_##Name; }        \
  [[nodiscard]] Type            Name() const { return b_##Name.value(); }      \
  Q_SIGNAL void                 Name##Changed();                               \
                                                                               \
private:                                                                       \
  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(                                        \
      LayerWindow, Type, b_##Name, InitialValue, &LayerWindow::Name##Changed);

  NS_LW_B(QString, wlrNamespace, )
  NS_LW_B(ns::layershell::LayerAnchor::Anchors, anchors, LayerAnchor::None)
  NS_LW_B(ns::layershell::KeyboardInteractivity::Enum, keyboardInteractivity,
          KeyboardInteractivity::OnDemand)
  NS_LW_B(ns::layershell::WlrLayer::Enum, layer, WlrLayer::Top)
  NS_LW_B(QMargins, margins, )
  NS_LW_B(ns::layershell::LayerAnchor::Anchor, exclusiveEdge, LayerAnchor::None)
  NS_LW_B(qint32, exclusionZone, 0)
  NS_LW_B(QSize, desiredSize, QSize(0, 0))
#undef NS_LW_B

public:
  ~LayerWindow() override;
  Q_DISABLE_COPY_MOVE(LayerWindow)

  static LayerWindow *get(QWindow *window);
  static LayerWindow *qmlAttachedProperties(QObject *object);

  bool eventFilter(QObject *watched, QEvent *event) override;

  [[nodiscard]] QScreen *screen() const;
  void                   setScreen(QScreen *screen);

signals:
  void screenChanged();

private:
  explicit LayerWindow(QWindow *window);
  void initializeShell();

  std::unique_ptr<LayerWindowPrivate> d;
};
} // namespace ns::layershell
