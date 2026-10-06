#include "userdata.h"

#include <pwd.h>
#include <qobject.h>
#include <qproperty.h>
#include <qvarlengtharray.h>
#include <unistd.h>

#include "helpermacros.h"

namespace ns::services {
AUTO_MEYERS_SINGLETON_QML_IMPL(UserData)

UserData::UserData(QObject *parent) : QObject(parent) { fetchUserData(); }

QBindable<QString> UserData::bindableName() const { return &b_name; }
QBindable<QString> UserData::bindableRealName() const { return &b_realName; }
QBindable<quint32> UserData::bindableUid() const { return &b_uid; }

void UserData::fetchUserData() {
  static auto pwuidbufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (pwuidbufsize == -1) pwuidbufsize = 8192;
  QVarLengthArray<char, 8192> pwuidbuf(pwuidbufsize);

  passwd  pwuid{};
  passwd *pwuidresult = nullptr;
  auto    r           = getpwuid_r(
      getuid(), &pwuid, pwuidbuf.data(), pwuidbuf.size(), &pwuidresult);

  if (pwuidresult != nullptr) {
    QScopedPropertyUpdateGroup scope;
    b_name     = pwuid.pw_name;
    b_realName = pwuid.pw_gecos;
    b_uid      = static_cast<quint32>(pwuid.pw_uid);
  }
}
} // namespace ns::services
