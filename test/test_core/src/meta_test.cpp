// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "print_writer.hpp"
#include "gtest/gtest.h"
#include "test_types.hpp"

namespace atlas::test
{

TEST(MetaTest, Method)
{
    auto meta_class = meta_class_of<MyClass>();
    {
        auto method = meta_class->find_method("get_id");
        MyClass my_class(true, 20, 0.0f);
        int32 result;
        method->invoke(&my_class, param_pack_null, &result);
        EXPECT_EQ(result, 20);
    }
    {
        auto method = meta_class->find_method("add");
        double result;
        auto a = 5;
        auto b = 7.2;
        auto pack = pack_arguments(a, b);
        method->invoke(pack, &result);
        EXPECT_EQ(result, 12.2);
    }
}

TEST(MetaTest, Property)
{
    auto meta_class = meta_class_of<MyClass>();
    MyClass my_class { true, 20, 5.0f, EMyEnum::Two, "hello" };
    {
        auto prop = meta_class->find_property("b_");

        EXPECT_NE(nullptr, meta_cast<BoolProperty>(prop));
        EXPECT_TRUE(meta_cast<BoolProperty>(prop)->get_value(&my_class));
        meta_cast<BoolProperty>(prop)->set_value(&my_class, false);
        EXPECT_FALSE(my_class.b_);
    }
    {
        auto prop = meta_class->find_property("id_");

        EXPECT_NE(nullptr, meta_cast<IntProperty>(prop));
        EXPECT_EQ(meta_cast<IntProperty>(prop)->get_value(&my_class), 20);
        meta_cast<IntProperty>(prop)->set_value(&my_class, 10);
        EXPECT_EQ(my_class.id_, 10);
    }
    {
        auto prop = meta_class->find_property("f_");

        EXPECT_NE(nullptr, meta_cast<FloatPointProperty>(prop));
        EXPECT_EQ(meta_cast<FloatPointProperty>(prop)->get_value(&my_class), 5.0);
        meta_cast<FloatPointProperty>(prop)->set_value(&my_class, 2.0);
        EXPECT_EQ(my_class.f_, 2.0);
    }
    {
        auto prop = meta_class->find_property("enumerator_");

        EXPECT_NE(nullptr, meta_cast<EnumProperty>(prop));
        auto enum_prop = meta_cast<EnumProperty>(prop);
        EXPECT_TRUE(enum_prop->get_enum() == meta_enum_of<EMyEnum>());
        EXPECT_EQ(meta_cast<EnumProperty>(prop)->get_value(&my_class), static_cast<int64>(EMyEnum::Two));
        meta_cast<EnumProperty>(prop)->set_value(&my_class, static_cast<int64>(EMyEnum::One));
        EXPECT_EQ(my_class.enumerator_, EMyEnum::One);
    }
    {
        auto prop = meta_class->find_property("str_");

        EXPECT_NE(nullptr, meta_cast<StringProperty>(prop));
        EXPECT_EQ(meta_cast<StringProperty>(prop)->get_value(&my_class), "hello");
        meta_cast<StringProperty>(prop)->set_value(&my_class, "world");
        EXPECT_EQ(my_class.str_, "world");
    }
}

TEST(MetaTest, Class)
{
    {
        for (auto it = MetaClass::create_class_iterator(); it; ++it)
        {
            printf("Class: %s\n", it->name().to_string().data());
        }
    }

    auto animal_class = meta_class_of<AnimalBase>();
    auto children = animal_class->get_children(true);

    {
        for (auto child : children)
        {
            auto method = child->find_method("make_sound");
            EXPECT_TRUE(!!method);

            void* instance = Memory::malloc(child->class_size());

            child->construct(instance);

            method->invoke(instance, param_pack_null, nullptr);

            child->destruct(instance);

            Memory::free(instance);
        }
    }

    {
        AnimalBase* animal = new Cat({2022, 4, 1}, "pangzi");
        EXPECT_TRUE(animal->meta_class() == meta_class_of<Cat>());
        auto cat_class = animal->meta_class();

        auto prop = cat_class->find_property("birth_day_");
        EXPECT_TRUE(!!prop && prop->is<ClassProperty>());

        auto date_prop = meta_cast<ClassProperty>(prop);
        auto date_class = date_prop->get_class();
        EXPECT_TRUE(date_class == meta_class_of<DateTime>());

        {
            for (auto it = cat_class->create_property_iterator(); it; ++it)
            {
                printf("property: %s\n", it->name().to_string().data());
            }
        }

        {
            auto year_prop = date_class->find_property("year");
            uint64 value = meta_cast<UIntProperty>(year_prop)->get_value(date_prop->get_class_address(animal));
            EXPECT_EQ(value, 2022);
        }
        {
            auto month_prop = date_class->find_property("month");
            uint64 value = meta_cast<UIntProperty>(month_prop)->get_value(date_prop->get_class_address(animal));
            EXPECT_EQ(value, 4);
        }
        {
            auto month_prop = date_class->find_property("day");
            uint64 value = meta_cast<UIntProperty>(month_prop)->get_value(date_prop->get_class_address(animal));
            EXPECT_EQ(value, 1);
        }

        delete animal;
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

TEST(MetaTest, Serialize)
{
    {
        Cat cat({2022, 4, 1}, "pangzi");
        auto cls = cat.meta_class();
        PrintWriter writer;
        cls->serialize(writer, &cat);
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