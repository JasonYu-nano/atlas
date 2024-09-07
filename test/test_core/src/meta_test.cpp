// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"
#include "test_types.hpp"

namespace atlas::test
{

TEST(MetaTest, Method)
{
    auto meta_class = meta_class_of<MyStruct>();
    {
        auto method = meta_class->get_method("get_id");
        MyStruct my_struct(true, 20, 0.0f);
        int32 result;
        method->invoke(&my_struct, param_pack_null, &result);
        EXPECT_EQ(result, 20);
    }
    {
        auto method = meta_class->get_method("add");
        double result;
        auto pack = pack_arguments(5, 7.2);
        method->invoke( pack, &result);
        EXPECT_EQ(result, 12.2);
    }
}

TEST(MetaTest, Property)
{
    auto meta_class = meta_class_of<MyStruct>();
    MyStruct my_struct { true, 20, 5.0f, EMyEnum::Two, "hello" };
    {
        auto prop = meta_class->get_property("b_");

        EXPECT_NE(nullptr, meta_cast<BoolProperty>(prop));
        EXPECT_TRUE(meta_cast<BoolProperty>(prop)->get_value(&my_struct));
        meta_cast<BoolProperty>(prop)->set_value(&my_struct, false);
        EXPECT_FALSE(my_struct.b_);
    }
    {
        auto prop = meta_class->get_property("id_");

        EXPECT_NE(nullptr, meta_cast<IntProperty>(prop));
        EXPECT_EQ(meta_cast<IntProperty>(prop)->get_value(&my_struct), 20);
        meta_cast<IntProperty>(prop)->set_value(&my_struct, 10);
        EXPECT_EQ(my_struct.id_, 10);
    }
    {
        auto prop = meta_class->get_property("f_");

        EXPECT_NE(nullptr, meta_cast<FloatPointProperty>(prop));
        EXPECT_EQ(meta_cast<FloatPointProperty>(prop)->get_value(&my_struct), 5.0);
        meta_cast<FloatPointProperty>(prop)->set_value(&my_struct, 2.0);
        EXPECT_EQ(my_struct.f_, 2.0);
    }
    {
        auto prop = meta_class->get_property("enumerator_");

        EXPECT_NE(nullptr, meta_cast<EnumProperty>(prop));
        EXPECT_EQ(meta_cast<EnumProperty>(prop)->get_value(&my_struct), static_cast<int64>(EMyEnum::Two));
        meta_cast<EnumProperty>(prop)->set_value(&my_struct, static_cast<int64>(EMyEnum::One));
        EXPECT_EQ(my_struct.enumerator_, EMyEnum::One);
    }
    {
        auto prop = meta_class->get_property("str_");

        EXPECT_NE(nullptr, meta_cast<StringProperty>(prop));
        EXPECT_EQ(meta_cast<StringProperty>(prop)->get_value(&my_struct), "hello");
        meta_cast<StringProperty>(prop)->set_value(&my_struct, "world");
        EXPECT_EQ(my_struct.str_, "world");
    }
}

TEST(MetaTest, Class)
{
    auto meta_class = meta_class_of<MyStruct>();
    {
        EXPECT_TRUE(MetaClass::find_class("MyStruct") == meta_class);
    }
    {
        EXPECT_TRUE(meta_class->is_derived_from(meta_class_of<BaseStruct>()));
        EXPECT_TRUE(meta_class->has_implement_interface(meta_class_of<ITestInterface>()));
        EXPECT_FALSE(meta_class_of<ITestInterface>()->has_implement_interface(meta_class_of<ITestInterface>()));
    }
    {
        void* buffer = Memory::malloc(meta_class->class_size());
        meta_class->construct(buffer);
        EXPECT_EQ(static_cast<MyStruct*>(buffer)->f_, 0.0f);
        meta_class->destruct(buffer);
        Memory::free(buffer);
    }
}

TEST(MetaTest, Enum)
{
    auto meta_enum = meta_enum_of<EMyEnum>();
    {
        EXPECT_TRUE(MetaEnum::find_enum("EMyEnum") == meta_enum);
    }
    {
        EXPECT_EQ(meta_enum->size(), 3);
        EXPECT_EQ(meta_enum->get_enum_value(0), 0);
        EXPECT_EQ(meta_enum->get_value_by_name("One"), 1);
        EXPECT_EQ(meta_enum->get_name_by_value(2), "Two");
    }
    {
#if WITH_EDITOR
        auto field = meta_enum->get_field(1);
        auto md = field->get_meta("ToolTip");
        EXPECT_TRUE(std::get<String>(md) == "Enum One");
#endif
    }
}

TEST(MetaTest, Offset)
{
    class MyClass
    {
    public:
        virtual int get_i() = 0;

        int i_{0};
    };

    class MyClassChild : public MyClass
    {
    public:
        int get_i() override { return 0; }

        bool b_{false};
    };

    EXPECT_EQ(offsetof(MyClassChild, i_), offsetof(MyClass, i_));
    EXPECT_EQ(offsetof(MyClass, i_), 8);
    EXPECT_EQ(offsetof(MyClassChild, b_), 16);

    MetaType::variant_type var;
    EXPECT_TRUE(var.index() == 0);
    var = 1;
    EXPECT_TRUE(var.index() == 1);
    var = "String";
    EXPECT_TRUE(var.index() == 2);
}

}// namespace atlas::test