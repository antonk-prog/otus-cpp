#include <gtest/gtest.h>
#include "lib.h"
TEST(VersionTest, GoogleTest) {
    EXPECT_GT(version(), 0);
}