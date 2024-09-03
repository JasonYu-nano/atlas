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

struct META() BaseStruct
{
    GEN_CLASS_BODY(BaseStruct)
};

struct TEST_CORE_API META(Serializable, ToolTip = "Only for test", MaxSize = 1) MyStruct
{
    GEN_CLASS_BODY(MyStruct)

    META(Serializable)
    bool b = false;

    META()
    int32 id {0};

    META()
    float f = 0.0f;

    META(Serializable)
    EMyEnum enumerator = EMyEnum::None;

    META()
    String str = "";

    META()
    StringName name = "";

    META()
    int32 get_id() const { return id; }

    META()
    static auto add(int32 a, double b) -> double
    {
        return static_cast<double>(a) + b;
    }

    static void add(void* instance, ParamPack& packed_params, void* result)
    {
        int32& a = *reinterpret_cast<int32*>(packed_params[0]);
        double& b = *reinterpret_cast<double*>(packed_params[1]);
        *static_cast<double*>(result) = static_cast<MyStruct*>(instance)->add(a, b);
    }
};

}// namespace atlas::test