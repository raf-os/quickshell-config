#include "systemtime.h"

#include <qdatetime.h>
#include <qobject.h>
#include <qtimer.h>
#include <qtypes.h>

namespace ns::utils {
SystemTime::SystemTime(QObject *parent) : QObject(parent) {
  QObject::connect(
      &m_timer, &QTimer::timeout, this, &SystemTime::onTimerTimeout);
  b_enabled.onValueChanged([this] { this->update(); });
  update();
}

QDateTime SystemTime::date() const { return m_currentTime; }

void SystemTime::onTimerTimeout() {
  setTime(m_targetTime);
  schedule(m_targetTime);
}

void SystemTime::update() {
  if (b_enabled.value()) {
    setTime(QDateTime::fromMSecsSinceEpoch(0));
    schedule(QDateTime::fromMSecsSinceEpoch(0));
  } else {
    m_timer.stop();
  }
}

void SystemTime::setTime(const QDateTime &target) {
  auto curTime  = QDateTime::currentDateTime();
  auto offset   = curTime.msecsTo(target);
  m_currentTime = offset > -500 && offset < 500 ? target : curTime;

  emit dateChanged();
}

void SystemTime::schedule(const QDateTime &target) {
  auto current  = QDateTime::currentDateTime();
  auto offset   = current.msecsTo(target);
  auto nextTime = offset > 0 && offset < 500 ? target : current;
  nextTime      = nextTime.addSecs(1);

  auto delay = current.msecsTo(nextTime);
  m_timer.start(static_cast<qint32>(delay));
  m_targetTime = nextTime;
}
} // namespace ns::utils
