#include <gtest/gtest.h>
#include <prelude.hpp>

#include "name.hpp"
#include "type.hpp"

using namespace mangekyou;

TEST(KindTest, star) {
  EXPECT_EQ(Kind::Star(), Kind::Star());
  EXPECT_EQ(Kind::Star().to_string(), "Kind::Star");
}

TEST(KindTest, arrow) {
  EXPECT_EQ(Kind::mkUnary(), Kind::mkUnary());
  EXPECT_EQ(Kind::mkUnary().to_string(), "Kind::Arrow(Kind::Star, Kind::Star)");
}
