// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "test_types.gen.hpp"

namespace atlas::test
{

using namespace atlas;
using namespace atlas::test;

enum class META() EMyEnum : uint32
{
    None,
    One,
    Two,
};

struct META() BaseStruct {};

struct TEST_CORE_API META(Serializable, ToolTip = "Only for test", MaxSize = 1) MyStruct
{
    META()
    bool b = false;

    META()
    int32 id {0};

    META()
    float f = 0.0f;

    META()
    EMyEnum enumerator = EMyEnum::None;

    META()
    String str = "";

    META()
    StringName name = "";

    META()
    static int32 get_static_id() { return 10; }

    META()
    int32 get_id() { return id; }

    META()
    static auto add(int32 a, int32 b) -> double
    {
        return static_cast<double>(a) + b;
    }

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

    NODISCARD MetaClass* meta_class() const
    {
        return meta_class_of<MyStruct>();
    }
};

}// namespace atlas::test