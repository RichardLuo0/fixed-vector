export module fixedVector:Iter;

import std;

namespace richardLuo {
template <class _Iter>
class Iter {
 public:
  typedef _Iter iterator_type;
  typedef typename std::iterator_traits<iterator_type>::value_type value_type;
  typedef typename std::iterator_traits<iterator_type>::difference_type
      difference_type;
  typedef typename std::iterator_traits<iterator_type>::pointer pointer;
  typedef typename std::iterator_traits<iterator_type>::reference reference;
  typedef typename std::iterator_traits<iterator_type>::iterator_category
      iterator_category;
#if _LIBCPP_STD_VER >= 20
  typedef std::contiguous_iterator_tag iterator_concept;
#endif

 private:
  iterator_type _i;

 public:
  Iter() noexcept : _i() {}
  template <
      class _Up,
      std::enable_if<std::is_convertible<_Up, iterator_type>::value, int> = 0>
  Iter(const Iter<_Up> &__u) noexcept : _i(__u.base()) {}
  explicit Iter(iterator_type __x) noexcept : _i(__x) {}
  reference operator*() const noexcept { return *_i; }
  pointer operator->() const noexcept { return _i; }
  Iter &operator++() noexcept {
    ++_i;
    return *this;
  }
  Iter operator++(int) noexcept {
    Iter __tmp(*this);
    ++(*this);
    return __tmp;
  }

  Iter &operator--() noexcept {
    --_i;
    return *this;
  }
  Iter operator--(int) noexcept {
    Iter __tmp(*this);
    --(*this);
    return __tmp;
  }
  Iter operator+(difference_type __n) const noexcept {
    Iter __w(*this);
    __w += __n;
    return __w;
  }
  Iter &operator+=(difference_type __n) noexcept {
    _i += __n;
    return *this;
  }
  Iter operator-(difference_type __n) const noexcept { return *this + (-__n); }
  Iter &operator-=(difference_type __n) noexcept {
    *this += -__n;
    return *this;
  }
  reference operator[](difference_type __n) const noexcept { return _i[__n]; }

  iterator_type base() const noexcept { return _i; }
};
}  // namespace richardLuo

export namespace richardLuo {
template <class _Iter1>
constexpr bool operator==(const Iter<_Iter1> &__x,
                          const Iter<_Iter1> &__y) noexcept {
  return __x.base() == __y.base();
}

template <class _Iter1, class _Iter2>
constexpr bool operator==(const Iter<_Iter1> &__x,
                          const Iter<_Iter2> &__y) noexcept {
  return __x.base() == __y.base();
}

template <class _Iter1>
constexpr bool operator<(const Iter<_Iter1> &__x,
                         const Iter<_Iter1> &__y) noexcept {
  return __x.base() < __y.base();
}

template <class _Iter1, class _Iter2>
constexpr bool operator<(const Iter<_Iter1> &__x,
                         const Iter<_Iter2> &__y) noexcept {
  return __x.base() < __y.base();
}

template <class _Iter1>
constexpr bool operator!=(const Iter<_Iter1> &__x,
                          const Iter<_Iter1> &__y) noexcept {
  return !(__x == __y);
}

template <class _Iter1, class _Iter2>
constexpr bool operator!=(const Iter<_Iter1> &__x,
                          const Iter<_Iter2> &__y) noexcept {
  return !(__x == __y);
}

template <class _Iter1>
constexpr bool operator>(const Iter<_Iter1> &__x,
                         const Iter<_Iter1> &__y) noexcept {
  return __y < __x;
}

template <class _Iter1, class _Iter2>
constexpr bool operator>(const Iter<_Iter1> &__x,
                         const Iter<_Iter2> &__y) noexcept {
  return __y < __x;
}

template <class _Iter1>
constexpr bool operator>=(const Iter<_Iter1> &__x,
                          const Iter<_Iter1> &__y) noexcept {
  return !(__x < __y);
}

template <class _Iter1, class _Iter2>
constexpr bool operator>=(const Iter<_Iter1> &__x,
                          const Iter<_Iter2> &__y) noexcept {
  return !(__x < __y);
}

template <class _Iter1>
constexpr bool operator<=(const Iter<_Iter1> &__x,
                          const Iter<_Iter1> &__y) noexcept {
  return !(__y < __x);
}

template <class _Iter1, class _Iter2>
constexpr bool operator<=(const Iter<_Iter1> &__x,
                          const Iter<_Iter2> &__y) noexcept {
  return !(__y < __x);
}

template <class _Iter1, class _Iter2>
constexpr
#ifndef _LIBCPP_CXX03_LANG
    auto
    operator-(const Iter<_Iter1> &__x,
              const Iter<_Iter2> &__y) noexcept -> decltype(__x.base() -
                                                            __y.base())
#else
    typename Iter<_Iter1>::difference_type
    operator-(const Iter<_Iter1> &__x, const Iter<_Iter2> &__y) noexcept
#endif  // C++03
{
  return __x.base() - __y.base();
}

template <class _Iter1>
constexpr Iter<_Iter1> operator+(typename Iter<_Iter1>::difference_type __n,
                                 Iter<_Iter1> __x) noexcept {
  __x += __n;
  return __x;
}
}  // namespace richardLuo
