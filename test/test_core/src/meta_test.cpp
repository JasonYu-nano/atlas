// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"
#include "meta/class.hpp"

namespace atlas::test
{

struct MyStruct
{
    int32 id = 0;

    static void get_static_id(void* instance, ParamPack& param_pack, void* result)
    {
        *static_cast<int32*>(result) = 10;
    }

    static void get_id(void* instance, ParamPack& param_pack, void* result)
    {
        *static_cast<int32*>(result) = static_cast<MyStruct*>(instance)->id;
    }

    static void add(void* instance, ParamPack& packed_params, void* result)
    {
        int32& a = *reinterpret_cast<int32*>(packed_params[0]);
        double& b = *reinterpret_cast<double*>(packed_params[1]);
        *static_cast<double*>(result) = static_cast<double>(a) + b;
    }
};

TEST(MetaTest, Method)
{
    {
        MyStruct my_struct { 20 };
        Method m(&MyStruct::get_id);
        int32 result;
        m.invoke(&my_struct, param_pack_null, &result);
        EXPECT_EQ(result, 20);
    }
    {
        Method m(&MyStruct::get_static_id, EMethodFlag::Static);
        int32 result;
        m.invoke( param_pack_null, &result);
        EXPECT_EQ(result, 10);
    }
    {
        Method m(&MyStruct::add, EMethodFlag::Static);
        auto args = std::make_tuple(5, 7.2);
        struct S
        {
            int32 a = 5;
            double b = 7.2;
        };
        auto sz = sizeof(std::tuple<int, double>);
        S s;
        double result;
        auto pack = pack_arguments(5, 7.2);
        m.invoke( pack, &result);
        EXPECT_EQ(result, 12.2);
    }
}

}// namespace atlas::test