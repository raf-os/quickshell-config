#include "pamqml.h"

#include <pwd.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qproperty.h>
#include <qvarlengtharray.h>
#include <unistd.h>

#include "pam/conversation.h"
#include "pam/process.h"
#include "pam/ptypes.h"

namespace ns::services::pam {
PamContext::PamContext(QObject *parent) : QObject(parent) {}

void PamContext::componentComplete() {
  m_isInitialized = true;

  if (m_targetActive) startConversation();
}

QBindable<QString> PamContext::bindableMessage() const { return &b_message; }
QBindable<bool>    PamContext::bindableMessageIsError() const {
  return &b_messageIsError;
}
QBindable<bool> PamContext::bindableIsResponseRequired() const {
  return &b_isResponseRequired;
}
QBindable<bool> PamContext::bindableIsResponseVisible() const {
  return &b_isResponseVisible;
}

QString PamContext::user() const { return m_user; }
void    PamContext::setUser(const QString &user) {
  if (m_user == user) return;

  if (isActive()) {
    qCCritical(logNSPam)
        << "Attempted changing user while a pam context was active.";
    return;
  }

  m_user = user;
  emit userChanged();
}

bool PamContext::isActive() const { return m_conversation != nullptr; }

void PamContext::startConversation() {
  if (!m_isInitialized || m_conversation != nullptr) return;

  b_messageIsError     = false;
  b_isResponseRequired = false;

  QString user;

  {
    auto confDirInfo = QFileInfo(m_configDir);
    if (!confDirInfo.isDir()) {
      qCCritical(logNSPam) << "Cannot start" << this
                           << "because provided directory" << m_configDir
                           << "is invalid.";
      m_targetActive = false;
      return;
    }

    auto confFilePath = QDir(m_configDir).filePath(m_config);
    auto confFileInfo = QFileInfo(confFilePath);
    if (!confFileInfo.isFile()) {
      qCCritical(logNSPam) << "Cannot start" << this
                           << "because provided config file" << confFilePath
                           << "is invalid.";
      m_targetActive = false;
      return;
    }

    static auto pwuidbufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (pwuidbufSize == -1) pwuidbufSize = 8192;
    QVarLengthArray<char, 8192> pwuidbuf(pwuidbufSize);

    passwd  pwuid{};
    passwd *pwuidResult = nullptr;

    if (m_user.isEmpty()) {
      auto r = getpwuid_r(
          getuid(), &pwuid, pwuidbuf.data(), pwuidbuf.size(), &pwuidResult);
      if (pwuidResult == nullptr) {
        qCCritical(logNSPam) << "Cannot start" << this << ":" << r;
        m_targetActive = false;
        return;
      }

      user = pwuid.pw_name;
    } else {
      auto r = getpwnam_r(m_user.toStdString().c_str(), &pwuid, pwuidbuf.data(),
          pwuidbuf.size(), &pwuidResult);
      if (pwuidResult == nullptr) {
        if (r == 0) {
          qCCritical(logNSPam)
              << "Unable to start" << this << ": user not found:" << m_user;
        } else {
          qCCritical(logNSPam) << "Unable to start" << this << ":" << r;
        }
        m_targetActive = false;
        return;
      }

      user = pwuid.pw_name;
    }
  }

  m_conversation = new PamConversation(this);
  QObject::connect(m_conversation, &PamConversation::completed, this,
      &PamContext::onCompleted);
  QObject::connect(
      m_conversation, &PamConversation::error, this, &PamContext::onError);
  QObject::connect(
      m_conversation, &PamConversation::message, this, &PamContext::onMessage);
  emit isActiveChanged();

  m_conversation->start(m_configDir, m_config, user);
}

void PamContext::abortConversation() {
  if (m_conversation == nullptr) return;

  m_targetActive = false;

  QObject::disconnect(m_conversation, nullptr, this, nullptr);
  m_conversation->deleteLater();
  m_conversation = nullptr;
  emit isActiveChanged();

  if (!b_message.value().isEmpty()) {
    b_message = QString();
  }

  // b_messageIsError     = false;
  // b_isResponseRequired = false;
}

void PamContext::start() {
  if (m_targetActive || isActive()) return;

  m_targetActive = true;
  startConversation();
}
void PamContext::abort() {
  if (!m_targetActive || !isActive()) return;

  m_targetActive = false;
  abortConversation();
}
void PamContext::respond(const QString &response) {
  if (isActive() && b_isResponseRequired.value()) {
    m_conversation->respond(response);
  } else {
    qCWarning(logNSPam) << "Attempted responding without need - ignoring.";
  }
}

void PamContext::onCompleted(PamResult::Enum result) {
  abortConversation();
  emit completed(result);
}

void PamContext::onError(PamError::Enum error) {
  abortConversation();
  emit this->error(error);
  emit completed(PamResult::Error);
}

void PamContext::onMessage(
    QString message, bool isChanged, bool isError, bool responseRequired) {
  {
    QScopedPropertyUpdateGroup scope;
    b_message            = message;
    b_messageIsError     = isError;
    b_isResponseRequired = responseRequired;
    b_isResponseVisible  = isChanged;
  }

  emit pamMessageReceived();
}
} // namespace ns::services::pam
