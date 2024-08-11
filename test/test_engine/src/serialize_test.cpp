// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"
#include "serialize/binary_archive.hpp"
#include "serialize/json_archive.hpp"

namespace atlas::test
{

struct MyStruct
{
    int8 i8 = 125;
    uint8 ui8 = 255;
    int16 i16 = -3000;
    uint16 ui16 = 0xffff;
    int32 i32 = -55667788;
    uint32 ui32 = 0xffffffff;
    int64 i64 = -55667788;
    uint64 ui64 = std::numeric_limits<uint64>::max();
    bool boolean = false;
    String str = "SomeString";
    StringName name = "StringName";

    friend void serialize(WriteStream& ws, const MyStruct& v)
    {
        ws << v.i8 << v.ui8 << v.i16 << v.ui16 << v.i32 << v.ui32
        << v.i64 << v.ui64 << v.boolean << v.str << v.name;
    }

    friend void deserialize(ReadStream& rs, MyStruct& v)
    {
        rs >> v.i8 >> v.ui8 >> v.i16 >> v.ui16 >> v.i32 >> v.ui32
        >> v.i64 >> v.ui64 >> v.boolean >> v.str >> v.name;
    }

    bool operator==(const MyStruct& o) const
    {
        return i8 == o.i8 && ui8 == o.ui8 && i16 == o.i16 && ui16 == o.ui16 && i32 == o.i32 && ui32 == o.ui32 &&
            i64 == o.i64 && ui64 == o.ui64 && boolean == o.boolean && str == o.str && name == o.name;
    }
};

TEST(SerializeTest, JsonArchive)
{
    JsonArchiveWriter writer;
    MyStruct a{};
    serialize(writer, a);

    JsonArchiveReader reader(writer.get_json());

    MyStruct b{};
    std::memset(&b, 0, sizeof(MyStruct));
    deserialize(reader, b);
    EXPECT_EQ(a, b);
}

TEST(SerializeTest, BinaryArchive)
{
    BinaryArchiveWriter writer;
    MyStruct a{};
    serialize(writer, a);

    BinaryArchiveReader reader(writer.get_buffer());

    MyStruct b{};
    std::memset(&b, 0, sizeof(MyStruct));
    deserialize(reader, b);
    EXPECT_EQ(a, b);
}

}