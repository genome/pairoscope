#include "io/DepthArray.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

TEST(DepthArray, construct_default) {
    DepthArray<long> dflt;
    EXPECT_TRUE(dflt.empty());
    EXPECT_EQ(0u, dflt.size());
    EXPECT_TRUE(dflt.sequence_name().empty());
    EXPECT_EQ(0u, dflt.begin_pos());
    EXPECT_EQ(0u, dflt.end_pos());
    EXPECT_EQ(0u, dflt.max_value());
}

TEST(DepthArray, construct) {
    DepthArray<unsigned> da("2", 5, 9);
    EXPECT_FALSE(da.empty());
    EXPECT_EQ(4u, da.size());
    EXPECT_EQ("2", da.sequence_name());
    EXPECT_EQ(5u, da.begin_pos());
    EXPECT_EQ(9u, da.end_pos());
    EXPECT_EQ(0u, da.max_value());
}

TEST(DepthArray, bounds) {
    EXPECT_THROW(DepthArray<int>("1", 1, 1), std::range_error);

    DepthArray<int> da("1", 11, 15);
    EXPECT_THROW(da.increment(10, 1), std::range_error);
    EXPECT_THROW(da.increment(15, 1), std::range_error);
    EXPECT_THROW(da.at_index(5), std::range_error);

    for (size_t i = 11; i < 15; ++i) {
        EXPECT_NO_THROW(da.increment(i, 1));
        EXPECT_NO_THROW(da.increment(i, 2));
        EXPECT_EQ(3, da[i]);
        EXPECT_EQ(3, da.at_index(i - da.begin_pos()));
    }
}

TEST(DepthArray, cap_value) {
    DepthArray<unsigned char> da("1", 0, 10);
    unsigned char max_minus_one = std::numeric_limits<unsigned char>::max() - 1;
    da.increment(0, max_minus_one);
    EXPECT_EQ(max_minus_one, da[0]);
    da.increment(0, 1);
    EXPECT_EQ(max_minus_one + 1, da[0]);
    da.increment(0, 2);
    EXPECT_EQ(max_minus_one + 1, da[0]);
    da.increment(0, 3);
    EXPECT_EQ(max_minus_one + 1, da[0]);

}

TEST(DepthArray, max_value) {
    DepthArray<unsigned> da("1", 0, 10);
    EXPECT_EQ(0u, da.max_value());

    da.increment(3, 2);
    EXPECT_EQ(2u, da.max_value());

    da.increment(4, 1);
    EXPECT_EQ(2u, da.max_value());

    da.increment(3, 1);
    EXPECT_EQ(3u, da.max_value());

    da.increment(5, 9);
    EXPECT_EQ(9u, da.max_value());
}

TEST(DepthArray, at_index) {
    DepthArray<unsigned short> da("5", 100, 110);
    for (unsigned short i = 0; i < 10; ++i) {
        da.increment(da.begin_pos() + i, i);
        EXPECT_EQ(i, da.at_index(i));
    }
}
