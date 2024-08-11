// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "serialize/json_archive.hpp"

namespace atlas
{

class SerializeExample
{
    struct MyStruct
    {
        int8 i8 = 0;
        bool is_class = false;
        String name = "MyStruct";

        friend void serialize(WriteStream& ws, const MyStruct& v)
        {
            ws << v.i8 << v.is_class << v.name;
        }

        friend void deserialize(ReadStream& rs, MyStruct& v)
        {
            rs >> v.i8;
            rs >> v.is_class;
            rs >> v.name;
        }
    };
public:
    void initialize();
    void deinitialize();
};

}// namespace atlas