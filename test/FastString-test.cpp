#include <gtest/gtest.h>

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

TEST(FastStringTest, fastcmp) {
  auto* one = new std::string("hi");
  auto* two = new std::string("hi");
  EXPECT_EQ(FastString("hi"), FastString("hi"));
  EXPECT_EQ(FastString("hi"), FastString(*one));
  EXPECT_EQ(FastString(*one), FastString(*two));

  auto* three = new std::string("bye");
  auto* four = new std::string("bye");
  EXPECT_EQ(FastString(*three), FastString(*four));
  EXPECT_EQ(FastString("bye"), FastString(*three));
  EXPECT_EQ(FastString("bye"), FastString(*four));
}

TEST(FastStringTest, nfastcmp) {
  auto* one = new std::string("hi");
  auto* two = new std::string("bye");
  EXPECT_NE(FastString("hi"), FastString("bye"));
  EXPECT_NE(FastString("bye"), FastString(*one));
  EXPECT_NE(FastString("hi"), FastString(*two));
  EXPECT_NE(FastString(*one), FastString(*two));
}
