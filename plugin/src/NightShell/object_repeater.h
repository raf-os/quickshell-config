#pragma once

#include <QList>
#include <QMap>
#include <QObject>
#include <QQmlComponent>
#include <QQmlParserStatus>
#include <QScreen>

namespace ns {
class ObjectRepeaterBase : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("Use one of the derived classes")
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(QQmlComponent *delegate MEMBER m_delegate)
  Q_CLASSINFO("DefaultProperty", "delegate")

public:
  explicit ObjectRepeaterBase(QObject *parent = nullptr);

  void classBegin() override {}
  void componentComplete() override;

  [[nodiscard]] QQmlComponent *delegate();

  virtual void updateInstances() = 0;

private:
  QQmlComponent *m_delegate = nullptr;
};

class ScreenObjectRepeater : public ObjectRepeaterBase {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QList<QScreen *> screens READ screens WRITE setScreens NOTIFY
                 screensChanged)

public:
  explicit ScreenObjectRepeater(QObject *parent = nullptr);

  void updateInstances() override;

  [[nodiscard]] QList<QScreen *> screens() const;
  void                           setScreens(const QList<QScreen *> &value);

signals:
  void screensChanged();

private:
  QList<QScreen *>           m_model;
  QMap<QScreen *, QObject *> m_instances;
};
} // namespace ns
