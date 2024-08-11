// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "serialize/stream.hpp"

namespace atlas
{

class SerializeExample
{
    struct MyStruct
    {
        int8 i8 = 125;
        int16 i16 = -3000;
        bool is_class = false;
        String name = "MyStruct";
        StringName owner = "SerializeExample";

        friend void serialize(WriteStream& ws, const MyStruct& v)
        {
            ws << v.i8 << v.i16 << v.is_class << v.name << v.owner;
        }

        friend void deserialize(ReadStream& rs, MyStruct& v)
        {
            rs >> v.i8;
            rs >> v.i16;
            rs >> v.is_class;
            rs >> v.name;
            rs >> v.owner;
        }

        bool operator==(const MyStruct& other) const
        {
            return i8 == other.i8 && i16 == other.i16 && name == other.name && owner == other.owner;
        }
    };
public:
    void initialize();
    void deinitialize();

private:
    void serialize_to_json();
    void serialize_to_binary();
};

}// namespace atlas