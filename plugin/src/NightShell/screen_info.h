#pragma once

#include <memory>
#include <optional>

#include <QObject>
#include <QScreen>
#include <QtQmlIntegration>

#include "iscreen_data.h"

namespace ns {
class ScreenInfo : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(ShellScreen)
  QML_UNCREATABLE("")

  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString model READ model CONSTANT)
  Q_PROPERTY(QString serial READ serial CONSTANT)

  Q_PROPERTY(QScreen *screen READ screen CONSTANT)

public:
  explicit ScreenInfo(QScreen *screen);

  [[nodiscard]] QString name() const;
  [[nodiscard]] QString model() const;
  [[nodiscard]] QString serial() const;

  [[nodiscard]] QScreen     *screen();
  [[nodiscard]] bool         hasData() const;
  [[nodiscard]] IScreenData *data();

  template <typename T>
    requires std::is_base_of_v<IScreenData, T>
  std::optional<T *> getData() {
    if (!m_data) {
      m_data = std::make_unique<T>();
    }
    T *cast = qobject_cast<T *>(m_data.get());
    if (!cast) return {};
    return cast;
  };

private:
  QScreen                     *m_screen;
  std::unique_ptr<IScreenData> m_data = nullptr;
};
} // namespace ns
