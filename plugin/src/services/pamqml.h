#pragma once

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qqmlparserstatus.h>
#include <qtmetamacros.h>

#include "pam/conversation.h"
#include "pam/ptypes.h"

namespace ns::services::pam {
class PamContext : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(QString message READ default NOTIFY messageChanged BINDABLE
          bindableMessage)
  Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
  Q_PROPERTY(bool isActive READ isActive NOTIFY isActiveChanged)

  Q_PROPERTY(bool messageIsError READ default NOTIFY messageIsErrorChanged
          BINDABLE bindableMessageIsError)
  Q_PROPERTY(
      bool isResponseRequired READ default NOTIFY isResponseRequiredChanged
          BINDABLE bindableIsResponseRequired)
  Q_PROPERTY(bool isResponseVisible READ default NOTIFY isResponseVisibleChanged
          BINDABLE bindableIsResponseVisible)

public:
  explicit PamContext(QObject *parent = nullptr);

  void classBegin() override {};
  void componentComplete() override;

  void startConversation();
  void abortConversation();

  Q_INVOKABLE void start();
  Q_INVOKABLE void abort();
  Q_INVOKABLE void respond(const QString &response);

  [[nodiscard]] bool isActive() const;

  [[nodiscard]] QBindable<QString> bindableMessage() const;

  [[nodiscard]] QBindable<bool> bindableMessageIsError() const;
  [[nodiscard]] QBindable<bool> bindableIsResponseRequired() const;
  [[nodiscard]] QBindable<bool> bindableIsResponseVisible() const;

  [[nodiscard]] QString user() const;
  void                  setUser(const QString &user);

signals:
  void completed(ns::services::pam::PamResult::Enum result);
  void error(ns::services::pam::PamError::Enum error);

  void pamMessageReceived();

  void messageChanged();
  void userChanged();
  void isActiveChanged();
  void messageIsErrorChanged();
  void isResponseRequiredChanged();
  void isResponseVisibleChanged();

private slots:
  void onCompleted(PamResult::Enum result);
  void onError(PamError::Enum error);
  void onMessage(
      QString message, bool isChanged, bool isError, bool responseRequired);

private:
  bool             m_isInitialized = false;
  bool             m_targetActive  = false;
  PamConversation *m_conversation  = nullptr;

  QString m_config    = "login";
  QString m_configDir = "/etc/pam.d";
  QString m_user;

#define B(Type, Name)                                                          \
  Q_OBJECT_BINDABLE_PROPERTY(                                                  \
      PamContext, Type, b_##Name, &PamContext::Name##Changed)

  B(QString, message)
  B(bool, messageIsError)
  B(bool, isResponseRequired)
  B(bool, isResponseVisible)
#undef B
};
} // namespace ns::services::pam
