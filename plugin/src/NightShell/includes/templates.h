#include <algorithm>
#include <cstddef>

#include <qbytearrayview.h>
#include <qlatin1stringview.h>

template <std::size_t N> struct TString {
  char data[N]{};

  constexpr TString(const char (&str)[N]) { std::copy_n(str, N, data); }

  constexpr operator const char *() const noexcept { return this->data; }
  operator QLatin1StringView() const { return QLatin1StringView(data, N); }
};
