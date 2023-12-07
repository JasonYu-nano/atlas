// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "core_def.hpp"
#include "utility/compression_pair.hpp"

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
}

}