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
        MyStruct my_struct { true, 20, 0.0f };
        int32 result;
        method->invoke(&my_struct, param_pack_null, &result);
        EXPECT_EQ(result, 20);
    }
    {
        auto method = meta_class->get_method("get_static_id");
        int32 result;
        method->invoke( param_pack_null, &result);
        EXPECT_EQ(result, 10);
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
        auto prop = meta_class->get_property("b");

        EXPECT_NE(nullptr, meta_cast<BoolProperty>(prop));
        EXPECT_TRUE(meta_cast<BoolProperty>(prop)->get_value(&my_struct));
        meta_cast<BoolProperty>(prop)->set_value(&my_struct, false);
        EXPECT_FALSE(my_struct.b);
    }
    {
        auto prop = meta_class->get_property("id");

        EXPECT_NE(nullptr, meta_cast<IntProperty>(prop));
        EXPECT_EQ(meta_cast<IntProperty>(prop)->get_value(&my_struct), 20);
        meta_cast<IntProperty>(prop)->set_value(&my_struct, 10);
        EXPECT_EQ(my_struct.id, 10);
    }
    {
        auto prop = meta_class->get_property("f");

        EXPECT_NE(nullptr, meta_cast<FloatPointProperty>(prop));
        EXPECT_EQ(meta_cast<FloatPointProperty>(prop)->get_value(&my_struct), 5.0);
        meta_cast<FloatPointProperty>(prop)->set_value(&my_struct, 2.0);
        EXPECT_EQ(my_struct.f, 2.0);
    }
    {
        auto prop = meta_class->get_property("enumerator");

        EXPECT_NE(nullptr, meta_cast<EnumProperty>(prop));
        EXPECT_EQ(meta_cast<EnumProperty>(prop)->get_value(&my_struct), static_cast<int64>(EMyEnum::Two));
        meta_cast<EnumProperty>(prop)->set_value(&my_struct, static_cast<int64>(EMyEnum::One));
        EXPECT_EQ(my_struct.enumerator, EMyEnum::One);
    }
    {
        auto prop = meta_class->get_property("str");

        EXPECT_NE(nullptr, meta_cast<StringProperty>(prop));
        EXPECT_EQ(meta_cast<StringProperty>(prop)->get_value(&my_struct), "hello");
        meta_cast<StringProperty>(prop)->set_value(&my_struct, "world");
        EXPECT_EQ(my_struct.str, "world");
    }
}

TEST(MetaTest, Class)
{
    auto meta_class = meta_class_of<MyStruct>();
    {
        void* buffer = Memory::malloc(meta_class->class_size());
        meta_class->construct(buffer);
        EXPECT_EQ(static_cast<MyStruct*>(buffer)->f, 0.0f);
        meta_class->destruct(buffer);
        Memory::free(buffer);
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