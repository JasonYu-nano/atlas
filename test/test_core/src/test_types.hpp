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

    META(Temporary)
    bool b_ = false;

    META()
    int32 id_ {0};

    META()
    float f_ = 0.0f;

    META(Temporary)
    EMyEnum enumerator_ = EMyEnum::None;

    META()
    String str_ = "";

    META()
    StringName name_ = "";
};

struct META() DateTime
{
    GEN_CLASS_BODY(DateTime)
    DateTime() = default;
    DateTime(uint16 y, uint8 m, uint8 d) : year(y), month(m), day(d) {}

    META()
    uint16 year = 0;

    META()
    uint8 month = 0;

    META()
    uint8 day = 0;
};

class META() IAnimal
{
    GEN_CLASS_BODY(IAnimal)
public:
    virtual ~IAnimal() = default;

    META()
    virtual void make_sound() const = 0;
};

class META() AnimalBase : public IAnimal
{
    GEN_CLASS_BODY(AnimalBase)

public:
    AnimalBase() = default;
    AnimalBase(DateTime date, String name) : birth_day_(date), name_(std::move(name)) {}

    META()
    NODISCARD String get_name() const { return name_; }

    META()
    NODISCARD const DateTime& get_birth_day() const { return birth_day_; }

private:
    META()
    DateTime birth_day_ {};

    META()
    String name_ {};
};

class META() Cat : public AnimalBase
{
    GEN_CLASS_BODY(Cat);

public:
    Cat() = default;
    Cat(DateTime date, String name, bool is_male) : AnimalBase(date, name), is_male_(is_male) {}

    void make_sound() const override
    {
        printf("meow meow!\n");
    }

private:
    META()
    bool is_male_{ false };
};

class META() Dog : public AnimalBase
{
    GEN_CLASS_BODY(Dog);

public:
    void make_sound() const override
    {
        printf("bark!\n");
    }

private:
    META()
    bool is_male_{ false };
};

}// namespace atlas::test