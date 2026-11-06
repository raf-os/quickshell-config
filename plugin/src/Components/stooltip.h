#pragma once

#include <qobject.h>
#include <qpointer.h>
#include <qproperty.h>
#include <qqmlcomponent.h>
#include <qqmlintegration.h>
#include <qqmlparserstatus.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwindow.h>

#include "tooltip_provider.h"

namespace ns::components {
class SToolTip;

// class OTooltipProvider : public QObject, public QQmlParserStatus {
//   Q_OBJECT
//   QML_ELEMENT
//   Q_INTERFACES(QQmlParserStatus)
//
//   Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY
//           delegateChanged)
//
//   Q_CLASSINFO("DefaultProperty", "delegate")
//
// public:
//   explicit OTooltipProvider(QObject *parent = nullptr);
//   ~OTooltipProvider() override;
//
//   void classBegin() override {}
//   void componentComplete() override;
//
//   [[nodiscard]] bool isActive() const;
//
//   [[nodiscard]] QBindable<bool> bindableReady() const;
//   [[nodiscard]] bool            ready() const;
//
//   [[nodiscard]] QQmlComponent *delegate();
//   void                         setDelegate(QQmlComponent *delegate);
//
//   void requestOwnership(SToolTip *owner);
//   void releaseOwnership(SToolTip *self);
//
// signals:
//   void readyChanged();
//   void delegateChanged();
//   void itemChanged();
//
// private slots:
//   void onComponentDestroyed();
//   void onOwnerDestroyed();
//   void onOwnerTextChanged();
//   void onGraceTimerTimeout();
//   void tryAttach(QQuickWindow *window);
//
// private:
//   QQmlComponent         *m_delegate     = nullptr;
//   QQuickItem            *m_component    = nullptr;
//   SToolTip              *m_owner        = nullptr;
//   QPointer<QQuickWindow> m_parentWindow = nullptr;
//   TooltipProxyWindow    *m_proxyWindow;
//   QQuickItem            *m_contentItem;
//   QTimer                *m_graceTimer;
//
//   bool mapContentToOwner();
//
//   Q_OBJECT_BINDABLE_PROPERTY(
//       OTooltipProvider, bool, b_ready, &OTooltipProvider::ready)
// };

class SToolTip : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(bool active READ default WRITE default NOTIFY activeChanged
          BINDABLE bindableActive)
  Q_PROPERTY(QString text READ default WRITE default NOTIFY textChanged BINDABLE
          bindableText)
  Q_PROPERTY(int delay READ default WRITE default NOTIFY delayChanged BINDABLE
          bindableDelay)
  Q_PROPERTY(
      TooltipAnchor::Enum anchor READ default WRITE default NOTIFY anchorChanged
          BINDABLE bindableAnchor)
  Q_PROPERTY(TooltipAlignment::Enum alignment READ default WRITE default NOTIFY
          alignmentChanged BINDABLE bindableAlignment)
  Q_PROPERTY(qreal offset READ default WRITE default NOTIFY offsetChanged
          BINDABLE bindableOffset)

public:
  explicit SToolTip(QObject *parent = nullptr);

  void classBegin() override {}
  void componentComplete() override;

  [[nodiscard]] QBindable<bool>                   bindableActive();
  [[nodiscard]] QBindable<QString>                bindableText();
  [[nodiscard]] QBindable<int>                    bindableDelay();
  [[nodiscard]] QBindable<TooltipAnchor::Enum>    bindableAnchor();
  [[nodiscard]] QBindable<TooltipAlignment::Enum> bindableAlignment();
  [[nodiscard]] QBindable<qreal>                  bindableOffset();

  [[nodiscard]] QQuickItem *parentItem();

public slots:
  void ownershipLost();

signals:
  void activeChanged();
  void textChanged();
  void delayChanged();
  void anchorChanged();
  void alignmentChanged();
  void offsetChanged();

private slots:
  void onReadyToRequest();
  void onActiveChanged();
  void onWindowChanged(QQuickWindow *window);
  void onWindowDestroyed();

private:
  bool          m_valid = false;
  QTimer       *m_timer;
  QQuickWindow *m_parentWindow = nullptr;

  Q_OBJECT_BINDABLE_PROPERTY(SToolTip, bool, b_active, &SToolTip::activeChanged)
  Q_OBJECT_BINDABLE_PROPERTY(SToolTip, QString, b_text, &SToolTip::textChanged)
  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(
      SToolTip, int, b_delay, 1000, &SToolTip::delayChanged)
  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SToolTip, TooltipAnchor::Enum, b_anchor,
      TooltipAnchor::Bottom, &SToolTip::anchorChanged)
  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SToolTip, TooltipAlignment::Enum,
      b_alignment, TooltipAlignment::Middle, &SToolTip::alignmentChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      SToolTip, qreal, b_offset, &SToolTip::offsetChanged)
};
} // namespace ns::components
