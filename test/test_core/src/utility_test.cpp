// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "utility/compression_pair.hpp"
#include "utility/call_traits.hpp"
#include "utility/untyped_data.hpp"

namespace atlas::test
{

TEST(CallTraitsTest, UtilityTest)
{
    struct CT
    {

    };

    EXPECT_TRUE(std::is_const_v<CallTraits<uint64>::ParamType> && !std::is_reference_v<CallTraits<uint64>::ParamType>);
    EXPECT_TRUE(std::is_reference_v<CallTraits<CT>::ParamType>);
}

TEST(CompressionPairTest, UtilityTest)
{
    struct CPA
    {
    };

    struct CPB
    {
        int32 value = 0;
    };

    struct CPC
    {
        int64 value = 0;
    };

    struct CPD
    {
    };

    EXPECT_TRUE(sizeof(CompressionPair<CPA, CPB>) == 4);
    EXPECT_TRUE(sizeof(CompressionPair<CPB, CPA>) == 4);
    EXPECT_TRUE(sizeof(CompressionPair<CPB, CPC>) == 16);
    EXPECT_TRUE(sizeof(CompressionPair<CPA, CPD>) == 1);

    CompressionPair<CPB, CPC> pair_0;
    EXPECT_TRUE(pair_0.First().value == 0 && sizeof(pair_0.First().value) == 4);
    EXPECT_TRUE(pair_0.Second().value == 0 && sizeof(pair_0.Second().value) == 8);

    CompressionPair<CPA, CPD> pair_1;
    EXPECT_TRUE(static_cast<void*>(&pair_1.First()) != static_cast<void*>(&pair_1.Second()));

    CompressionPair<CPA, CPA> pair_2;
    EXPECT_TRUE(static_cast<void*>(&pair_2.First()) == static_cast<void*>(&pair_2.Second()));

    CompressionPair<CPB, CPB> pair_3;
    EXPECT_TRUE(static_cast<void*>(&pair_3.First()) != static_cast<void*>(&pair_3.Second()));
}

TEST(UntypedDataTest, UtilityTest)
{
    static_assert(alignof(UntypedData<bool>) == 1 && sizeof(UntypedData<bool>) == 1);
    static_assert(alignof(UntypedData<int16>) == 2 && sizeof(UntypedData<int16>) == 2);
    static_assert(alignof(UntypedData<int32>) == 4 && sizeof(UntypedData<int32>) == 4);
    static_assert(alignof(UntypedData<int64>) == 8 && sizeof(UntypedData<int64>) == 8);

    struct UT
    {
        alignas(64) bool a;
        alignas(128) int32 b;
    };

    static_assert(alignof(UntypedData<UT>) == 128 && sizeof(UntypedData<UT>) == 256);
}

}