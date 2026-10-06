#pragma once

#include <cstddef>
#include <string>

#include <qloggingcategory.h>
#include <qtclasshelpermacros.h>
#include <security/pam_appl.h>

#include "ptypes.h"

namespace ns::services::pam {
Q_DECLARE_LOGGING_CATEGORY(logNSPam)

struct PamRequestFlags {
  bool echo;
  bool error;
  bool responseRequired;
};

class PamPipes {
public:
  explicit PamPipes() = default;
  explicit PamPipes(int fdIn, int fdOut);
  ~PamPipes();
  Q_DISABLE_COPY_MOVE(PamPipes)

  [[nodiscard]] bool        readBytes(char *buffer, size_t length) const;
  [[nodiscard]] bool        writeBytes(char *buffer, size_t length) const;
  [[nodiscard]] std::string readString(bool *ok) const;
  [[nodiscard]] bool        writeString(std::string &str) const;

  template <typename T> [[nodiscard]] inline bool readAsBytes(T *ptr) {
    return readBytes(reinterpret_cast<char *>(ptr), sizeof(T));
  }

  int fdIn  = 0;
  int fdOut = 0;
};

class PamProcess {
public:
  explicit PamProcess(bool shouldLog, int fdIn, int fdOut);
  PamExitCode exec(const char *configDir, const char *config, const char *user);
  void        sendCode(PamExitCode code);

private:
  static int conversation(int num_msg, const pam_message **msg,
      pam_response **resp, void *appdata_ptr);

  bool     m_shouldLog;
  PamPipes m_pipes;
};
} // namespace ns::services::pam
