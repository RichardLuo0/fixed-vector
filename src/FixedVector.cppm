module;
#include <cassert>

export module fixedVector;

import std;
import :Iter;

#define ASSERT(cond, msg) assert(cond || !(std::cerr << msg << std::endl))

export namespace richardLuo {
template <class T, class Allocator = std::allocator<T>>
class fixed_vector {
 public:
  typedef T value_type;
  typedef Allocator allocator_type;
  typedef std::allocator_traits<allocator_type> __alloc_traits;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef typename __alloc_traits::size_type size_type;
  typedef typename __alloc_traits::difference_type difference_type;
  typedef typename __alloc_traits::pointer pointer;
  typedef typename __alloc_traits::const_pointer const_pointer;
  typedef Iter<pointer> iterator;
  typedef Iter<const_pointer> const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

 private:
  Allocator allocator;
  size_type _size = 0;
  size_type _capacity = 0;
  value_type *buf = nullptr;

 public:
  fixed_vector() = delete;
  explicit fixed_vector(const allocator_type &a) : fixed_vector(0, a) {};
  explicit fixed_vector(size_type capacity,
                        const allocator_type &a = allocator_type())
      : allocator(a), _capacity(capacity) {
    buf = allocator.allocate(capacity);
  };  // C++14
  fixed_vector(size_type capacity, const value_type &value,
               const allocator_type &a = allocator_type())
      : fixed_vector(capacity, a) {
    for (; _size < _capacity; _size++) {
      std::construct_at(buf + _size, value);
    }
  };
  template <class InputIterator>
  fixed_vector(InputIterator first, InputIterator last,
               const allocator_type &a = allocator_type())
      : fixed_vector(last - first, a) {
    for (; _size < _capacity; _size++) {
      std::construct_at(buf + _size, *(first + _size));
    }
  };
  fixed_vector(const std::initializer_list<value_type> &il,
               const allocator_type &a = allocator_type())
      : fixed_vector(il.begin(), il.end(), a) {};

  // Like std::initializer_list but without move or copy
 private:
  template <class... Args>
  void _emplace_back_unsafe(const std::tuple<Args...> &args) {
    std::construct_at(buf + _size++, std::forward<Args>(std::get<0>(args))...);
  }

 public:
  template <class... Values>
  fixed_vector(std::piecewise_construct_t, const allocator_type &a,
               Values &&...values)
      : fixed_vector(sizeof...(values), a) {
    (_emplace_back_unsafe(std::forward<Values>(values)), ...);
  }
  template <class... Values>
  fixed_vector(std::piecewise_construct_t, Values &&...values)
      : fixed_vector(sizeof...(values), allocator_type()) {}
  template <class... Values>
  fixed_vector(const allocator_type &a, Values &&...values)
      : fixed_vector(sizeof...(values), a) {
    (std::construct_at(buf + _size++, std::forward<Values>(values)), ...);
  };
  template <class... Values>
  static auto create(Values &&...values) {
    return fixed_vector(allocator_type(), std::forward<Values>(values)...);
  };

#if _LIBCPP_STD_VER >= 23
  template <std::ranges::range R>
  constexpr fixed_vector(std::from_range_t, R &&rg,
                         const Allocator & = Allocator())
      : fixed_vector(rg.begin(), rg.end()) {}  // C++23
#endif

  fixed_vector(const fixed_vector &x)
      : fixed_vector(x.begin(), x.end(), x.allocator) {};
  fixed_vector(fixed_vector &&x) noexcept(
      std::is_nothrow_move_constructible<allocator_type>::value) {
    allocator = x.allocator;
    buf = x.buf;
    _size = x._size;
    _capacity = x._capacity;
    x.buf = nullptr;
    x._capacity = 0;
    x._size = 0;
  };

  ~fixed_vector() {
    clear();
    allocator.deallocate(buf, _capacity);
  };

  fixed_vector &operator=(const fixed_vector &x) {
    ASSERT(x._size <= _capacity,
           "fixed_vector copy assignment called with a bigger vector");
    clear();
    assign(x);
    allocator = x.allocator;
    return *this;
  };
  fixed_vector &operator=(fixed_vector &&x) noexcept(
      allocator_type::propagate_on_container_move_assignment::value ||
      allocator_type::is_always_equal::value) {
    clear();
    allocator.deallocate(buf, _capacity);
    allocator = x.allocator;
    buf = x.buf;
    _size = x._size;
    _capacity = x._capacity;
    x.buf = nullptr;
    x._capacity = 0;
    x._size = 0;
    return *this;
  };  // C++17
  fixed_vector &operator=(std::initializer_list<value_type> il) {
    ASSERT(il.size() <= _capacity,
           "list assignment is called on a smaller vector");
    clear();
    assign(il);
    return *this;
  };

  template <class InputIterator>
  void assign(InputIterator first, InputIterator last) {
    ASSERT(last - first <= _capacity,
           "fixed_vector copy constructor called with a larger vector");
    clear();
    for (; _size < last - first; _size++) {
      std::construct_at(buf + _size, *(first + _size));
    }
  };
  template <std::ranges::range R>
  constexpr void assign_range(R &&rg) {
    return assign(rg.begin(), rg.end());
  }  // C++23
  void assign(size_type n, const value_type &u) {
    clear();
    for (; _size < n; _size++) {
      std::construct_at(buf + _size, u);
    }
  };
  void assign(std::initializer_list<value_type> il) {
    return assign(il.begin(), il.end());
  };

  allocator_type get_allocator() const noexcept { return allocator; }

  iterator begin() noexcept { return iterator(buf); };
  const_iterator begin() const noexcept { return const_iterator(buf); }
  iterator end() noexcept { return iterator(buf + _size); };
  const_iterator end() const noexcept { return const_iterator(buf + _size); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  };
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  };

  const_iterator cbegin() const noexcept { return begin(); };
  const_iterator cend() const noexcept { return end(); };
  const_reverse_iterator crbegin() const noexcept { return rbegin(); };
  const_reverse_iterator crend() const noexcept { return rend(); };

  size_type size() const noexcept { return _size; };
  size_type max_size() const noexcept { return allocator.max_size(); };
  size_type capacity() const noexcept { return _capacity; };
  bool empty() const noexcept { return _size == 0; };

  reference operator[](size_type n) { return buf[n]; };
  const_reference operator[](size_type n) const { return buf[n]; };
  reference at(size_type n) {
    if (n < 0 && n >= _size)
      throw std::out_of_range(std::to_string(n) + " is out of range");
    return buf[n];
  };
  const_reference at(size_type n) const {
    if (n < 0 && n >= _size)
      throw std::out_of_range(std::to_string(n) + " is out of range");
    return buf[n];
  };

  reference front() { return *begin(); };
  const_reference front() const { return *begin(); };
  reference back() { return *end(); };
  const_reference back() const { return *end(); };

  value_type *data() noexcept { return buf; };
  const value_type *data() const noexcept { return buf; };

  void push_back(const value_type &x) {
    ASSERT(_size < _capacity,
           "fixed_vector is out of capacity when push_back()");
    std::construct_at(buf + _size++, x);
  };
  void push_back(value_type &&x) {
    ASSERT(_size < _capacity,
           "fixed_vector is out of capacity when push_back()");
    std::construct_at(buf + _size++, std::move(x));
  };
  template <class... Args>
  reference emplace_back(Args &&...args) {
    ASSERT(_size < _capacity,
           "fixed_vector is out of capacity when emplace_back()");
    return *(std::construct_at(buf + _size++, std::forward<Args>(args)...));
  };  // reference in C++17
  template <std::ranges::range R>
  constexpr void append_range(R &&rg) {
    insert(end(), rg.begin(), rg.end());
  }  // C++23
  void pop_back() {
    ASSERT(_size > 0, "fixed_vector::pop_back() called on an empty vector");
    std::destroy_at(buf + _size--);
  };

  template <class... Args>
  reference emplace_at(size_type n, Args &&...args) {
    if (n < 0 && n >= _size)
      throw std::out_of_range(std::to_string(n) + " is out of range");
    std::destroy_at(buf + n);
    return *(std::construct_at(buf + n, std::forward<Args>(args)...));
  }

  template <class... Args>
  iterator emplace(const_iterator position, Args &&...args) {
    size_type n = position - cbegin();
    ASSERT(n >= 0 && n <= _size, std::to_string(n) + " is out of capacity");
    if (n < _size) {
      ASSERT(_size + 1 <= _capacity,
             "fixed_vector is out of capacity when emplace()");
      iterator oldEnd = end();
      _size++;
      std::move_backward(iterator(buf + n), oldEnd, end());
    }
    return iterator(std::construct_at(buf + n, std::forward<Args>(args)...));
  };
  iterator insert(const_iterator position, const value_type &x) {
    return emplace(position, x);
  };
  iterator insert(const_iterator position, value_type &&x) {
    return emplace(position, x);
  }
  iterator insert(const_iterator position, size_type n, const value_type &x) {
    size_type n2 = position - cbegin();
    ASSERT(n2 >= 0 && n2 <= _size, std::to_string(n2) + " is out of capacity");
    iterator it(buf + n2);
    if (n2 < _size) {
      ASSERT(_size + n <= _capacity,
             "fixed_vector is out of capacity when insert()");
      iterator oldEnd = end();
      _size += n;
      std::move_backward(it, oldEnd, end());
    }
    std::fill_n(it, n, x);
    return it;
  }
  template <class InputIterator>
  iterator insert(const_iterator position, InputIterator first,
                  InputIterator last) {
    size_type n = last - first;
    size_type n2 = position - cbegin();
    ASSERT(n2 >= 0 && n2 <= _size, std::to_string(n2) + " is out of capacity");
    iterator it(buf + n2);
    if (n2 < _size) {
      ASSERT(_size + n <= _capacity,
             "fixed_vector is out of capacity when emplace()");
      iterator oldEnd = end();
      _size += n;
      std::move_backward(it, oldEnd, end());
    }
    std::copy(first, last, it);
    return it;
  };
  template <std::ranges::range R>
  constexpr iterator insert_range(const_iterator position, R &&rg) {
    return insert(position, rg.begin(), rg.end());
  }  // C++23
  iterator insert(const_iterator position,
                  std::initializer_list<value_type> il) {
    insert(position, il.begin(), il.end());
  };

  iterator erase(const_iterator position) {
    std::move(position + 1, this->cend(), position);
    _size--;
    return position;
  }
  iterator erase(const_iterator first, const_iterator last) {
    difference_type n = last - first;
    std::copy(last, this->cend(), first);
    _size -= n;
    return first;
  }

  void clear() noexcept {
    std::destroy(begin(), end());
    _size = 0;
  };

  void swap(fixed_vector &x) noexcept(
      std::allocator_traits<
          allocator_type>::propagate_on_container_swap::value ||
      std::allocator_traits<allocator_type>::is_always_equal::value) {
    std::swap(allocator, x.allocator);
    std::swap(_size, x._size);
    std::swap(_capacity, x._capacity);
    std::swap(buf, x.buf);
  };  // C++17
};

template <class T, class Allocator = std::allocator<T>>
using FixedVector = fixed_vector<T, Allocator>;
}  // namespace richardLuo
