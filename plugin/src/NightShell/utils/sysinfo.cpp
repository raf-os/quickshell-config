#include "sysinfo.h"

#include <qbuffer.h>
#include <qdir.h>
#include <qlogging.h>
#include <qobject.h>

namespace ns::utils {
SysInfo::SysInfo(QObject *parent) : QObject(parent) {
  auto osfile = QFile("/etc/os-release");
  if (!osfile.exists()) {
    return;
  }

  if (!osfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return;
  }

  QTextStream in(&osfile);
  while (!in.atEnd()) {
    QString line = in.readLine();
    auto    idx  = line.indexOf("=");
    if (idx == -1 || idx + 1 > line.length()) continue;

    auto vname = line.first(idx);
    auto vval  = line.sliced(idx + 1);
    if (vval.startsWith('"') && vval.endsWith('"')) {
      vval.remove(0, 1);
      vval.chop(1);
    }

    if (vname == "NAME") {
      m_name = vval;
    } else if (vname == "ID") {
      m_id = vval;
    } else if (vname == "LOGO") {
      m_iconName = vval;
    }
  }
}

QString SysInfo::name() const { return m_name; }
QString SysInfo::id() const { return m_id; }
QString SysInfo::iconName() const { return m_iconName; }
} // namespace ns::utils
