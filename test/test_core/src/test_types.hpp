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
    None    META(),
    One     META(ToolTip="Enum One"),
    Two,
};

class META() ITestInterface
{
    virtual int32 get_id() const = 0;
};

class META() BaseClass
{
    GEN_CLASS_BODY(BaseClass)

    int32 zzz;
};

class TEST_CORE_API META(ToolTip = "Only for test", MaxSize = 1) MyClass : public BaseClass, public ITestInterface
{
    GEN_CLASS_BODY(MyClass)
public:
    MyClass() = default;

    MyClass(bool b, int32 id, float f, EMyEnum e = EMyEnum::None, String s = "", StringName sn = "") :
        b_(b), id_(id), f_(f), enumerator_(e), str_(std::move(s)), name_(sn) {}

    META()
    int32 get_id() const override { return id_; }

    META()
    static auto add(int32 a, double b) -> double
    {
        return static_cast<double>(a) + b;
    }

    META(Serializable)
    bool b_ = false;

    META()
    int32 id_ {0};

    META()
    float f_ = 0.0f;

    META(Serializable)
    EMyEnum enumerator_ = EMyEnum::None;

    META()
    String str_ = "";

    META()
    StringName name_ = "";
};

}// namespace atlas::test