#include "gtest/gtest.h"
#include "name.hpp"

using namespace mangekyou::name;

TEST(FastStringTest, ctor) {
  EXPECT_EQ(FastString("hi").string(), "hi");
  EXPECT_EQ(FastString(std::string("hi")).string(), "hi");
  auto x = FastString("hi");
  EXPECT_EQ(FastString(x).string(), "hi");
}

TEST(FastStringTest, nctor) {
  EXPECT_NE(FastString("hi").string(), "bye");
  EXPECT_NE(FastString(std::string("hi")).string(), "bye");
  auto x = FastString("hi");
  EXPECT_NE(FastString(x).string(), "bye");
}

TEST(FastStringTest, dups) {
  auto a = FastString("hi");
  auto b = FastString("hi");
  EXPECT_EQ(a.str, b.str);
  EXPECT_EQ(a, b);
}
