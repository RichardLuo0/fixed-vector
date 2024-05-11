# fixed_vector
This library provides access to a fixed-capacity dynamically allocated array. The benefits are:
* Iterators and references never become invalid (except erased elements).
* High performance. It never allocated or deallocate memory during its lifetime and call move/copy constructor as fewer as possible.
* Dynamically allocated space. The capacity of this container is decided at runtime.
* It uses almost the same interface as std::vector. 

This container is especially useful if you have a class that can not copy or move. \
Consider the following code:
```cpp
std::vector<A> c;
c.reserve(10);
for (auto& args: something_from_outside)
  c.emplace_back(args);
```
This code requires `A` to be either move assignable or copy assignable. \
With fixed_vector, the code can rewrite as:
```cpp
fixed_vector<A> c(10);
for (auto& args: something_from_outside)
  c.emplace_back(args);
```
Keep in mind the size passed to constructor is capacity instead of actual size. This code generates 0 move and 0 copy.

Compared to `std::array`, the size of `fixed_vector` is decided by constructor instead of compile-time. \
Since iterators and references never become invalid. You may write code like this:
```cpp
using namespace richardLuo;

template<class T>
using Ref = std::reference_wrapper<T>;

struct Node {
  std::deque<Ref<Node>> children; 
};

auto createGraph() {
  fixed_vector<Node> g(10, Node());
  g.at(1).children.emplace_back(g.at(3));
  g.at(4).children.emplace_back(g.at(2));
  ...
  return g;
}
```
As long as `fixed_vector` is alive, the reference never gets invalid. Its more efficient than `std::deque` because it never allocates new space.

# Build from source
You will need:
* std module
* cmake or [make.cpp](https://github.com/RichardLuo0/make-dot-cpp)
* C++20 compatible compiler (I tested only clang)
## cmake
1. Run `cmake -B build -D stdPath=<the path to your std module>`
2. Run `cmake --build build`
## make.cpp
1. Setup std module as described in [here](https://github.com/RichardLuo0/make-dot-cpp)
2. Run `make.cpp`

## License
Distributed under the GPLv3.
