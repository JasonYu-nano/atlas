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

}