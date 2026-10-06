#pragma once

#include <qobject.h>
#include <qsocketnotifier.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <security/pam_appl.h>
#include <sys/types.h>

#include "process.h"
#include "ptypes.h"

namespace ns::services::pam {
struct PamConversation : public QObject {
  Q_OBJECT

public:
  explicit PamConversation(QObject *parent = nullptr);
  ~PamConversation() override;
  Q_DISABLE_COPY_MOVE(PamConversation)

  void start(
      const QString &configDir, const QString &config, const QString &user);
  void abort();
  void respond(const QString &response);

signals:
  void completed(PamResult::Enum result);
  void error(PamError::Enum error);
  void message(QString message, bool messageChanged, bool isError,
      bool responseRequired);

private slots:
  void onMessage();
  void onInternalError();

private:
  static pid_t createSubprocess(PamPipes *pipes, const QString &configDir,
      const QString &config, const QString &user);

  pid_t           m_childPid = 0;
  PamPipes        m_pipes;
  QSocketNotifier m_notifier{QSocketNotifier::Read};
};
} // namespace ns::services::pam
