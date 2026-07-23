#pragma once

// This is supposed to somewhat mimic C++26's inplace_vector. Quickshell is
// built with C++20 and C++26 is still being adopted by compilers so this will
// have to do.

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

template <typename T, size_t N> class InplaceVector {
  using value_type      = T;
  using pointer         = T *;
  using const_pointer   = const T *;
  using reference       = value_type &;
  using const_reference = const value_type &;
  using size_type       = size_t;
  using difference_size = std::ptrdiff_t;

public:
  reference operator[](size_type i) {
    static_assert(
        i <= N,
        "ns::utils::InplaceVector: Attempted to access value out of bounds.");
    return this->array[i];
  }
  const_reference operator[](size_type i) const {
    return const_cast<InplaceVector<T, N> *>(this)->operator[](i);
  }

  reference at(size_type pos) {
    if (pos < N) {
      return this->operator[](pos);
    } else {
      throw std::runtime_error(
          "Attempted to access out of bounds value from InplaceVector.");
    }
  }
  const_reference at(size_type pos) const {
    if (pos >= this->idx) {
      throw std::out_of_range("InplaceVector::at");
    }
    return this->operator[](pos);
  }

  void push(const_reference value) {
    if (this->idx < N) {
      this->array[this->idx] = value;
      ++this->idx;
    }
  }

  template <typename Self, typename ListPtr, typename IT> struct BaseIterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = int64_t;
    using value_type        = IT;
    using pointer           = IT *;
    using reference         = IT &;

    BaseIterator() = default;
    explicit BaseIterator(ListPtr list,
                          size_t  i)
        : list(list),
          i(i) {}

    reference operator*() const { return this->list->operator[](this->i); }
    pointer   operator->() const { return &**this; }

    Self &operator++() {
      ++this->i;
      return *static_cast<Self *>(this);
    }
    Self &operator--() {
      --this->i;
      return *static_cast<Self *>(this);
    }
    Self operator++(int) {
      auto v = *this;
      this->operator++();
      return v;
    }
    Self operator--(int) {
      auto v = *this;
      this->operator--();
      return v;
    }

    difference_type operator-(const Self &other) {
      return static_cast<int64_t>(this->i) - static_cast<int64_t>(other.i);
    }
    Self &operator+(difference_type offset) {
      return Self(this->list, static_cast<int64_t>(this->i) + offset);
    }

    [[nodiscard]] bool operator==(const Self &other) const {
      return this->list == other.list && this->i == other.i;
    }
    [[nodiscard]] bool operator!=(const Self &other) const {
      return !(*this == other);
    }

  private:
    ListPtr list = nullptr;
    size_t  i    = 0;
  };

  struct Iterator : public BaseIterator<Iterator,
                                        InplaceVector<value_type, N> *,
                                        value_type> {
    Iterator() = default;
    Iterator(InplaceVector<value_type,
                           N> *list,
             size_type         i)
        : BaseIterator<Iterator,
                       InplaceVector<value_type,
                                     N> *,
                       value_type>(list,
                                   i) {}
  };

  struct ConstIterator
      : public BaseIterator<ConstIterator,
                            const InplaceVector<value_type, N> *,
                            const value_type> {
    ConstIterator() = default;
    ConstIterator(const InplaceVector<value_type,
                                      N> *list,
                  size_type               i)
        : BaseIterator<ConstIterator,
                       const InplaceVector<value_type,
                                           N> *,
                       const value_type>(list,
                                         i) {}
  };

  [[nodiscard]] Iterator begin() { return Iterator(this, 0); }
  [[nodiscard]] Iterator end() { return Iterator(this, this->idx); }

  [[nodiscard]] ConstIterator begin() const { return ConstIterator(this, 0); }
  [[nodiscard]] ConstIterator end() const {
    return ConstIterator(this, this->idx);
  }

  [[nodiscard]] size_type length() const { return this->idx; }
  [[nodiscard]] bool      isEmpty() const { return this->idx == 0; }

  [[nodiscard]] const pointer data() const { return this->array.data(); }
  [[nodiscard]] size_type dataLength() const { return this->idx * sizeof(T); }

private:
  std::array<T, N> array{};
  size_t           idx;
};
