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

namespace ns::components {
class SToolTip;

namespace TooltipAnchor {
Q_NAMESPACE
QML_NAMED_ELEMENT(TooltipAnchor)

enum Enum { Top, Right, Bottom, Left };

Q_ENUM_NS(Enum)
} // namespace TooltipAnchor

namespace TooltipAlignment {
Q_NAMESPACE
QML_NAMED_ELEMENT(TooltipAlignment)

enum Enum { Start, Middle, End };

Q_ENUM_NS(Enum)
} // namespace TooltipAlignment

class TooltipProxyWindow : public QQuickWindow {
  Q_OBJECT
  QML_ANONYMOUS

public:
  explicit TooltipProxyWindow(QWindow *parent = nullptr);

  void setReference(QQuickItem *reference);
  void setTooltipContent(QQuickItem *content);
  void setAlignment(TooltipAlignment::Enum value);
  void setAnchor(TooltipAnchor::Enum value);

public slots:
  void reposition();

private slots:
  void onContentDestroyed();

private:
  QPointer<QQuickItem> m_reference;
  QQuickItem          *m_content = nullptr;

  int                    m_offset    = 0;
  TooltipAnchor::Enum    m_anchor    = TooltipAnchor::Bottom;
  TooltipAlignment::Enum m_alignment = TooltipAlignment::Middle;
};

class TooltipProvider : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY
          delegateChanged)

  Q_CLASSINFO("DefaultProperty", "delegate")

public:
  explicit TooltipProvider(QObject *parent = nullptr);
  ~TooltipProvider() override;

  void classBegin() override {}
  void componentComplete() override;

  [[nodiscard]] bool isActive() const;

  [[nodiscard]] QBindable<bool> bindableReady() const;
  [[nodiscard]] bool            ready() const;

  [[nodiscard]] QQmlComponent *delegate();
  void                         setDelegate(QQmlComponent *delegate);

  void requestOwnership(SToolTip *owner);
  void releaseOwnership(SToolTip *self);

signals:
  void readyChanged();
  void delegateChanged();
  void itemChanged();

private slots:
  void onComponentDestroyed();
  void onOwnerDestroyed();
  void onOwnerTextChanged();
  void onGraceTimerTimeout();
  void tryAttach(QQuickWindow *window);

private:
  QQmlComponent         *m_delegate     = nullptr;
  QQuickItem            *m_component    = nullptr;
  SToolTip              *m_owner        = nullptr;
  QPointer<QQuickWindow> m_parentWindow = nullptr;
  TooltipProxyWindow    *m_proxyWindow;
  QQuickItem            *m_contentItem;
  QTimer                *m_graceTimer;

  bool mapContentToOwner();

  Q_OBJECT_BINDABLE_PROPERTY(
      TooltipProvider, bool, b_ready, &TooltipProvider::ready)
};

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
          BINDABLE bindableAlignment)
  Q_PROPERTY(TooltipAlignment::Enum alignment READ default WRITE default NOTIFY
          alignmentChanged BINDABLE bindableAlignment)

public:
  explicit SToolTip(QObject *parent = nullptr);

  void classBegin() override {}
  void componentComplete() override;

  [[nodiscard]] QBindable<bool>                   bindableActive();
  [[nodiscard]] QBindable<QString>                bindableText();
  [[nodiscard]] QBindable<int>                    bindableDelay();
  [[nodiscard]] QBindable<TooltipAnchor::Enum>    bindableAnchor();
  [[nodiscard]] QBindable<TooltipAlignment::Enum> bindableAlignment();

  [[nodiscard]] QQuickItem *parentItem();

signals:
  void activeChanged();
  void textChanged();
  void delayChanged();
  void anchorChanged();
  void alignmentChanged();

private slots:
  void onReadyToRequest();
  void onActiveChanged();
  void onWindowChanged(QQuickWindow *window);
  void onWindowDestroyed();

private:
  bool             m_valid = false;
  QTimer          *m_timer;
  QQuickWindow    *m_parentWindow = nullptr;
  TooltipProvider *m_provider     = nullptr;

  Q_OBJECT_BINDABLE_PROPERTY(SToolTip, bool, b_active, &SToolTip::activeChanged)
  Q_OBJECT_BINDABLE_PROPERTY(SToolTip, QString, b_text, &SToolTip::textChanged)
  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(
      SToolTip, int, b_delay, 1000, &SToolTip::delayChanged)
  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SToolTip, TooltipAnchor::Enum, b_anchor,
      TooltipAnchor::Bottom, &SToolTip::anchorChanged)
  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SToolTip, TooltipAlignment::Enum,
      b_alignment, TooltipAlignment::Middle, &SToolTip::alignmentChanged)
};
} // namespace ns::components
