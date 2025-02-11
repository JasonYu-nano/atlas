// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "container/unordered_map.hpp"
#include "gtest/gtest.h"
#include "serialize/compact_binary_archive.hpp"
#include "utility/call_traits.hpp"
#include "utility/compression_pair.hpp"
#include "utility/guid.hpp"
#include "utility/untyped_data.hpp"

namespace atlas::test
{

TEST(UtilityTest, CallTraitsTest)
{
    struct CT
    {

    };

    EXPECT_TRUE(std::is_const_v<CallTraits<uint64>::param_type> && !std::is_reference_v<CallTraits<uint64>::param_type>);
    EXPECT_TRUE(std::is_reference_v<CallTraits<CT>::param_type>);
}

TEST(UtilityTest, CompressionPairTest)
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
    EXPECT_TRUE(pair_0.first().value == 0 && sizeof(pair_0.first().value) == 4);
    EXPECT_TRUE(pair_0.second().value == 0 && sizeof(pair_0.second().value) == 8);

    CompressionPair<CPA, CPD> pair_1;
    //EXPECT_TRUE(static_cast<void*>(&pair_1.First()) != static_cast<void*>(&pair_1.Second()));

    CompressionPair<CPA, CPA> pair_2;
    //EXPECT_TRUE(static_cast<void*>(&pair_2.First()) == static_cast<void*>(&pair_2.Second()));

    CompressionPair<CPB, CPB> pair_3;
    EXPECT_TRUE(static_cast<void*>(&pair_3.first()) != static_cast<void*>(&pair_3.second()));
}

TEST(UtilityTest, UntypedDataTest)
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

TEST(UtilityTest, GuidTest)
{
    auto id = GUID::new_guid();
    EXPECT_TRUE(id.is_valid());
    LOG_INFO(temp, "GUID is {}", id.to_string(GUID::EFormats::DigitsWithHyphensInBracesLowercase));

    UnorderedMap<GUID, int32> map;
    map.insert(id, 1);

    CompactBinaryArchiveWriter ws;
    ws << id;

    GUID new_id;
    CompactBinaryArchiveReader rs(ws.get_buffer());
    rs >> new_id;

    EXPECT_EQ(id, new_id);
}

}