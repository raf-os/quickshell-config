#include <algorithm>
#include <cstddef>

#include <bit>
#include <qbytearrayview.h>
#include <qchar.h>
#include <qhashfunctions.h>
#include <qlatin1stringview.h>
#include <qstringview.h>
#include <qtdeprecationdefinitions.h>

template <std::size_t N> struct TString {
  char data[N]{};

  constexpr TString(const char (&str)[N]) { std::copy_n(str, N, data); }

  constexpr operator const char *() const noexcept { return this->data; }
  operator QLatin1StringView() const { return QLatin1StringView(data, N); }
};

template <std::size_t N> struct TString16 {
  char16_t data[N]{};

  [[nodiscard]] constexpr const QChar *qCharPtr() const noexcept {
    return std::bit_cast<const QChar *>(&data);
  }

  constexpr TString16(const char16_t (&str)[N]) { std::copy_n(str, N, data); }
  [[nodiscard]] Q_ALWAYS_INLINE operator QString() const noexcept {
    return QString::fromRawData(qCharPtr(), static_cast<qsizetype>(N - 1));
  };

  [[nodiscard]] Q_ALWAYS_INLINE operator QStringView() const noexcept {
    return QStringView(qCharPtr(), static_cast<qsizetype>(N - 1));
  };
};
