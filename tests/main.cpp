#include <cassert>

import std;
import fixedVector;

using namespace richardLuo;

#define ASSERT(cond, msg) assert(cond || !(std::cerr << msg << std::endl))

struct Record {
  std::size_t copies = 0, moves = 0, copyAssigns = 0, moveAssigns = 0,
              destructs = 0;
} record;

void expect(Record &&exRc) {
#define GENERATE_ASSERT_RECORD(name)                                        \
  ASSERT(record.name == exRc.name, "" #name ": " +                          \
                                       std::to_string(record.name) +        \
                                       " != " + std::to_string(exRc.name)); \
  record.name = 0;
  GENERATE_ASSERT_RECORD(copies);
  GENERATE_ASSERT_RECORD(moves);
  GENERATE_ASSERT_RECORD(copyAssigns);
  GENERATE_ASSERT_RECORD(moveAssigns);
  GENERATE_ASSERT_RECORD(destructs);
#undef GENERATE_ASSERT_RECORD
}

struct A {
  int i;

  A(int i) : i(i) {};
  A(int i, int) : A(i) {};
  A(const A &o) {
    this->i = o.i;
    record.copies++;
  }
  A(A &&o) {
    this->i = o.i;
    record.moves++;
  }
  ~A() { record.destructs++; }

  A &operator=(const A &o) {
    this->i = o.i;
    record.copyAssigns++;
    return *this;
  }

  A &operator=(A &&o) {
    this->i = o.i;
    record.moveAssigns++;
    return *this;
  }
};

inline void expect(const std::ranges::range auto &c,
                   std::initializer_list<int> &&il) {
  auto it2 = il.begin();
  for (auto it = c.begin(); it != c.end();) {
    ASSERT(it->i == *it2,
           std::to_string(it->i) + " != " + std::to_string(*it2));
    it++;
    it2++;
  }
}

int main(int, const char **) {
  fixed_vector<A> fv(10);
  for (int i = 0; i < 5; i++) {
    fv.emplace_back(i);
  }
  expect(fv, {0, 1, 2, 3, 4});
  expect({0, 0, 0, 0, 0});

  fv.pop_back();
  expect(fv, {0, 1, 2, 3});
  expect({0, 0, 0, 0, 1});

  fv.emplace_at(1, 100);
  expect(fv, {0, 100, 2, 3});
  expect({0, 0, 0, 0, 1});

  fv.emplace(fv.cbegin(), 200);
  expect(fv, {200, 0, 100, 2, 3});
  expect({0, 0, 0, 4, 0});

  fv.insert(fv.cbegin(), 2, A(400));
  expect(fv, {400, 400, 200, 0, 100, 2, 3});
  expect({0, 0, 2, 5, 1});

  fv.clear();
  expect(fv, {});
  expect({0, 0, 0, 0, 7});

  fv.push_back(20);
  expect(fv, {20});
  expect({0, 1, 0, 0, 1});

  fixed_vector<A> fv2 = fixed_vector<A>::create(1, A(2), 3, 4);
  expect(fv2, {1, 2, 3, 4});
  expect({0, 1, 0, 0, 1});

  fixed_vector<A> fv3{1, 2, 3, 4};
  expect(fv2, {1, 2, 3, 4});
  expect({4, 0, 0, 0, 4});

  fixed_vector<A> fv4(fv2);
  expect(fv4, {1, 2, 3, 4});
  expect({4, 0, 0, 0, 0});

  fixed_vector<A> fv5(std::move(fv2));
  expect(fv5, {1, 2, 3, 4});
  expect({0, 0, 0, 0, 0});

  fixed_vector<A> fv6(std::piecewise_construct, std::forward_as_tuple(10, 2));
  expect(fv6, {10});
  expect({0, 0, 0, 0, 0});

  return 0;
}
