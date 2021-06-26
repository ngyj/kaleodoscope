#include "gtest/gtest.h"
#include "name.hpp"

using namespace mangekyou::name;

TEST(FastStringTest, ctor) {
  EXPECT_EQ(FastString("hi").string(), "hi");
}
TEST(FastStringTest, dups) {
  auto a = FastString("hi");
  auto b = FastString("hi");
  EXPECT_EQ(a.str, b.str);
}
