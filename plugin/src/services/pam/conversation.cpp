#include "conversation.h"

#include <array>
#include <cstdlib>

#include <qloggingcategory.h>
#include <qobject.h>
#include <qscopeguard.h>
#include <qsocketnotifier.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "process.h"
#include "ptypes.h"

namespace ns::services::pam {
PamConversation::PamConversation(QObject *parent) : QObject(parent) {}

PamConversation::~PamConversation() { abort(); }

void PamConversation::start(
    const QString &configDir, const QString &config, const QString &user) {
  m_childPid =
      PamConversation::createSubprocess(&m_pipes, configDir, config, user);

  if (m_childPid == 0) {
    qCCritical(logNSPam) << "Failed creating pam subprocess!";
    emit error(PamError::InternalError);
    return;
  }

  QObject::connect(&m_notifier, &QSocketNotifier::activated, this,
      &PamConversation::onMessage);
  m_notifier.setSocket(m_pipes.fdIn);
  m_notifier.setEnabled(true);
}

void PamConversation::abort() {
  if (m_childPid != 0) {
    qCDebug(logNSPam) << "Killing subprocess for" << this;
    kill(m_childPid, SIGKILL);
    waitpid(m_childPid, nullptr, 0);
    m_childPid = 0;
  }
}

void PamConversation::onInternalError() {
  if (m_childPid != 0) {
    abort();
    emit error(PamError::InternalError);
  }
}

void PamConversation::respond(const QString &response) {
  auto str = response.toStdString();
  if (!m_pipes.writeString(str)) {
    qCCritical(logNSPam) << "Failed to write response to subprocess.";
    onInternalError();
  }
}

void PamConversation::onMessage() {
  qCDebug(logNSPam) << "Received message from pam subprocess.";

  auto scope = qScopeGuard([this] {
    qCCritical(logNSPam) << "Failed to read subprocess request.";
    onInternalError();
  });

  auto type = PamEvent::Exit;
  auto ok   = m_pipes.readAsBytes(&type);
  if (!ok) return;

  if (type == PamEvent::Exit) {
    auto code = PamExitCode::OtherError;

    ok = m_pipes.readAsBytes(&code);
    if (!ok) return;

    qCDebug(logNSPam) << "Subprocess exited with code"
                      << static_cast<int>(code);

    switch (code) {
    case PamExitCode::Success:     emit completed(PamResult::Success); break;
    case PamExitCode::AuthFailed:  emit completed(PamResult::Failed); break;
    case PamExitCode::StartFailed: emit error(PamError::StartFailed); break;
    case PamExitCode::MaxTries:    emit completed(PamResult::MaxTries); break;
    case PamExitCode::PamError:    emit error(PamError::TryAuthFailed); break;
    case PamExitCode::OtherError:  emit error(PamError::InternalError); break;
    }

    waitpid(m_childPid, nullptr, 0);
    m_childPid = 0;
  } else if (type == PamEvent::Request) {
  }

  scope.dismiss();
  return;
}

pid_t PamConversation::createSubprocess(PamPipes *pipes,
    const QString &configDir, const QString &config, const QString &user) {
  // file descriptors
  auto toSubprocess   = std::array<int, 2>();
  auto fromSubprocess = std::array<int, 2>();

  if (pipe(toSubprocess.data()) == -1 || pipe(fromSubprocess.data()) == -1) {
    qCDebug(logNSPam) << "Failed to create pipes for subprocess.";
    return 0;
  }

  auto *configDirF = strdup(configDir.toStdString().c_str());
  auto *configF    = strdup(config.toStdString().c_str());
  auto *userF      = strdup(user.toStdString().c_str());
  auto  log        = logNSPam().isDebugEnabled();

  auto pid = fork();

  if (pid < 0) {
    qCDebug(logNSPam) << "Failed to fork for subprocess.";
  } else if (pid == 0) {
    // We're a subprocess
    close(toSubprocess[1]);   // write
    close(fromSubprocess[0]); // read

    {
      auto subprocess = PamProcess(log, toSubprocess[0], fromSubprocess[1]);
      auto code       = subprocess.exec(configDirF, configF, userF);
      subprocess.sendCode(code);
    }

    free(configDirF);
    free(configF);
    free(userF);

    _exit(0);
  } else {
    // We're the main process
    close(toSubprocess[0]);   // read
    close(fromSubprocess[1]); // write

    pipes->fdIn  = fromSubprocess[0];
    pipes->fdOut = toSubprocess[1];

    free(configDirF);
    free(configF);
    free(userF);

    return pid;
  }

  return -1;
}
} // namespace ns::services::pam
