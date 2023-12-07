// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "core_def.hpp"
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