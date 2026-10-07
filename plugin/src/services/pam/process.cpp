#include "process.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

#include <print>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qscopeguard.h>
#include <qtypes.h>
#include <qvarlengtharray.h>
#include <security/_pam_types.h>
#include <security/pam_appl.h>
#include <unistd.h>

#include "ptypes.h"

namespace ns::services::pam {
Q_LOGGING_CATEGORY(logNSPam, "ns.services.pam")

PamPipes::PamPipes(int fdIn, int fdOut) : fdIn(fdIn), fdOut(fdOut) {}

PamPipes::~PamPipes() {
  if (fdIn != 0) close(fdIn);
  if (fdOut != 0) close(fdOut);
}

bool PamPipes::readBytes(char *buffer, size_t length) const {
  size_t i = 0;

  while (i < length) {
    auto count = read(fdIn, buffer + i, length - i);
    if (count == -1 || count == 0) return false;

    i += count;
  }
  return true;
}

bool PamPipes::writeBytes(char *buffer, size_t length) const {
  size_t i = 0;

  while (i < length) {
    auto count = write(fdOut, buffer + i, length - i);
    if (count == -1 || count == 0) return false;

    i += count;
  }
  return true;
}

std::string PamPipes::readString(bool *ok) const {
  if (ok != nullptr) *ok = false;
  uint32_t length = 0;
  if (!readBytes(reinterpret_cast<char *>(&length), sizeof(length))) {
    return "";
  }
  QVarLengthArray<char, 1024> data(length);
  if (!readBytes(data.data(), length)) {
    return "";
  }
  if (ok != nullptr) *ok = true;
  return std::string(data.data(), length);
}

bool PamPipes::writeString(std::string &str) const {
  uint32_t length = str.length();
  if (!writeBytes(reinterpret_cast<char *>(&length), sizeof(length))) {
    return false;
  }
  return writeBytes(str.data(), str.length());
}

PamProcess::PamProcess(bool shouldLog, int fdIn, int fdOut)
    : m_shouldLog(shouldLog), m_pipes(fdIn, fdOut) {}

void PamProcess::sendCode(PamExitCode code) {
  auto guard = qScopeGuard([] { _exit(1); });
  auto event = PamEvent::Exit;
  auto ok =
      m_pipes.writeBytes(reinterpret_cast<char *>(&event), sizeof(PamEvent));
  if (!ok) return;

  ok = m_pipes.writeBytes(reinterpret_cast<char *>(&code), sizeof(PamExitCode));
  if (!ok) return;

  guard.dismiss();
  return;
}

PamExitCode PamProcess::exec(
    const char *configDir, const char *config, const char *user) {

  auto conv = pam_conv{.conv = &PamProcess::conversation, .appdata_ptr = this};

  pam_handle_t *handle = nullptr;
  auto result = pam_start_confdir(config, user, &conv, configDir, &handle);

  if (result != PAM_SUCCESS) {
    std::println("Failed starting pam conversation with error {} (code {})",
        pam_strerror(handle, result), result);
    return PamExitCode::StartFailed;
  }

  result           = pam_authenticate(handle, 0);
  PamExitCode code = PamExitCode::OtherError;

  switch (result) {
  case PAM_SUCCESS:  code = PamExitCode::Success; break;
  case PAM_AUTH_ERR: code = PamExitCode::AuthFailed; break;
  case PAM_MAXTRIES: code = PamExitCode::MaxTries; break;
  default:           code = PamExitCode::PamError; break;
  }

  result = pam_end(handle, result);
  handle = nullptr;

  return code;
}

int PamProcess::conversation(int num_msg, const pam_message **msg,
    pam_response **resp, void *appdata_ptr) {
  auto *self = static_cast<PamProcess *>(appdata_ptr);
  auto *responses =
      static_cast<pam_response *>(calloc(num_msg, sizeof(pam_response)));
  bool initialPrompt = true;

  auto scope = qScopeGuard([&responses] {
    free(responses);
    _exit(1);
  });

  for (auto i = 0; i < num_msg; i++) {
    const auto *message  = msg[i];
    auto       &response = responses[i];

    auto msgStr = std::string(message->msg);
    auto req =
        PamRequestFlags{.echo = message->msg_style != PAM_PROMPT_ECHO_OFF,
            .error            = message->msg_style == PAM_ERROR_MSG,
            .responseRequired = message->msg_style == PAM_PROMPT_ECHO_OFF ||
                                message->msg_style == PAM_PROMPT_ECHO_ON};

    auto event = PamEvent::Request;
    auto ok    = self->m_pipes.writeBytes(
        reinterpret_cast<char *>(&event), sizeof(PamEvent));

    if (!ok) return PAM_CONV_ERR;

    ok = self->m_pipes.writeBytes(
        reinterpret_cast<char *>(&req), sizeof(PamRequestFlags));

    if (!ok) return PAM_CONV_ERR;
    if (!self->m_pipes.writeString(msgStr)) return PAM_CONV_ERR;

    if (req.responseRequired) {
      auto ok = false;
      auto r  = self->m_pipes.readString(&ok);
      if (!ok) _exit(static_cast<int>(PamExitCode::OtherError));

      response.resp = strdup(r.c_str());
      initialPrompt = false;
    }
  }

  scope.dismiss();
  *resp = responses;
  return PAM_SUCCESS;
}
} // namespace ns::services::pam
