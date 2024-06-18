// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "string/string.hpp"
#include "string/string_name.hpp"

namespace atlas::test
{

TEST(StringTest, StringCtor)
{
    {
        String str;
        str.reserve(20);
        EXPECT_TRUE(str.length() == 0);
        EXPECT_TRUE(str.capacity() == 22);
    }

    {
        String str('a');
        EXPECT_TRUE(str.length() == 1);
        EXPECT_TRUE(str == String('a'));
    }

    {
        String str('b', 64);
        EXPECT_TRUE(str.length() == 64);
        EXPECT_TRUE(str == String('b', 64));
    }

    {
        auto raw_str = "this is an utf-8 string";
        String str(raw_str);
        EXPECT_TRUE(str.length() == std::char_traits<char>::length(raw_str));
        EXPECT_TRUE(str == String("this is an utf-8 string"));
    }

    {
        auto raw_str = "this is an utf-8 string";
        String str(raw_str, 16);
        EXPECT_TRUE(str.length() == 16);
        EXPECT_TRUE(str == String("this is an utf-8"));
    }

    {
        String str("💖");
        EXPECT_TRUE(str.count() == 1 && str.length() == 4);
    }

    {
        String str("你好atlas");
        EXPECT_TRUE(str.count() == 7 && str.length() == 11);
    }

    {
        String str_0("你好atlas");
        String str_1(std::move(str_0));
        EXPECT_TRUE(str_1.length() == 11);

        String str_2(std::move(str_1), 6, 5);
        EXPECT_TRUE(str_2 == String("atlas"));

        String str_3(str_2);
        EXPECT_TRUE(str_3 == String("atlas"));

        String str_4(str_2, 2);
        EXPECT_TRUE(str_4 == String("las"));
    }

    {
        String str_0 = "你好atlas";
        String str_1 = "test";
        str_1 = str_0;
        EXPECT_TRUE(str_1 == str_0);

        str_1 = std::move(str_0);
        EXPECT_TRUE(str_1 == "你好atlas");

        str_1 = "";
        EXPECT_TRUE(str_1.length() == 0);
    }
}

TEST(StringTest, StringFormat)
{
    {
        String name = "atlas";
        String str = String::format("my name is {0}", name);
        EXPECT_TRUE(str == "my name is atlas");
    }

    {
        String str = String::format("my id is {0}", 1);
        EXPECT_TRUE(str == "my id is 1");
    }

    {
        String name = "阿特拉斯";
        String str = String::format("my name is {0}", name);
        EXPECT_TRUE(str == "my name is 阿特拉斯");
    }
}

TEST(StringTest, StringAt)
{
    {
        String name = "阿特拉斯";
        EXPECT_TRUE(String::char_traits::to_int_type(name[3]) == 231);
        EXPECT_TRUE(name.code_point_at(1) == 29305);
        EXPECT_TRUE(name.code_point_at(4) == CodePoint::incomplete);
    }
}

TEST(StringTest, StringConvert)
{
    {
        String name = String::from_utf16(u"阿特拉斯");
        EXPECT_TRUE(name == "阿特拉斯");
    }

    {
        String name = String::from_utf32(U"阿特拉斯");
        EXPECT_TRUE(name == "阿特拉斯");
    }

    {
        String name = String::from(std::wstring(L"阿特拉斯"));
        EXPECT_TRUE(name == "阿特拉斯");
    }

    {
        String name = "Atlas";
        EXPECT_TRUE(name.to_lower() == "atlas");
        EXPECT_TRUE(name.to_lower().is_lower());
    }

    {
        String name = "atlas";
        EXPECT_TRUE(name.to_upper() == "ATLAS");
        EXPECT_TRUE(name.to_upper().is_upper());
    }
}

TEST(StringTest, StringEquals)
{
    {
        String name = "Atlas";
        EXPECT_FALSE(name.equals("atlas"));
        EXPECT_TRUE(name.equals("atlas", ECaseSensitive::Insensitive));
    }
}

TEST(StringTest, StringSearch)
{
    {
        String name = "Atlas阿特拉斯";
        EXPECT_FALSE(name.starts_with(String("atlas")));
        EXPECT_TRUE(name.starts_with("Atlas"));
        EXPECT_FALSE(name.starts_with(String("Atlas阿特拉斯A")));
        EXPECT_TRUE(name.starts_with("atlas", ECaseSensitive::Insensitive));
    }
    {
        String name = "阿特拉斯Atlas";
        EXPECT_FALSE(name.ends_with(String("atlas")));
        EXPECT_TRUE(name.ends_with("Atlas"));
        EXPECT_FALSE(name.ends_with(String("阿特拉斯AtlasA")));
        EXPECT_TRUE(name.ends_with("atlas", ECaseSensitive::Insensitive));
    }
    {
        String name = "Atlas";
        EXPECT_TRUE(name.index_of("la") == 2);
        EXPECT_TRUE(name.index_of("al") == INDEX_NONE);
        EXPECT_TRUE(name.index_of("at", ECaseSensitive::Insensitive) == 0);
        EXPECT_TRUE(name.index_of(String::view_type("at")) == INDEX_NONE);
        std::shared_ptr<String> a;
    }
}

TEST(StringTest, StringModify)
{
    {
        String name = "Atlas";
        String::view_type view = "阿特拉斯";
        name.append(view);
        EXPECT_TRUE(name == "Atlas阿特拉斯" && name.length() == 17);
    }
    {
        String name = "Atlas";
        name.prepend(String("阿特拉斯"));
        EXPECT_TRUE(name == "阿特拉斯Atlas" && name.length() == 17);
    }
    {
        String name = "Atlas";
        String new_name = name.concat("阿特拉斯");
        EXPECT_TRUE(name == "Atlas" && new_name == "Atlas阿特拉斯" && new_name.length() == 17);
    }
    {
        String name = "Atlas";
        name.insert(5, "阿特拉斯");
        EXPECT_TRUE(name == "Atlas阿特拉斯" && name.length() == 17);
    }
    {
        String name = "Atlas";
        name.insert(0, "阿特拉斯");
        EXPECT_TRUE(name == "阿特拉斯Atlas" && name.length() == 17);
    }
    {
        String name = "Atlas";
        name.remove(2, 2);
        EXPECT_TRUE(name == "Ats" && name.length() == 3);
    }
    {
        String name = "this is an utf-8 string";
        name.remove("is");
        EXPECT_TRUE(name == "th  an utf-8 string");
    }
    {
        String str = "this is a string";
        str.replace(8, 1, "an utf-8");
        EXPECT_TRUE(str == "this is an utf-8 string");
    }
    {
        String str = "cbegin is the const begin iterator of the string";
        str.replace("begin", "end");
        EXPECT_TRUE(str == "cend is the const end iterator of the string");
    }

}

TEST(StringTest, StringIter)
{
    {
        String str("atlas");
        EXPECT_TRUE(std::distance(str.begin(), str.end()) == 5);
        EXPECT_TRUE(std::distance(str.rbegin(), str.rend()) == 5);
        EXPECT_TRUE(std::distance(str.cbegin(), str.cend()) == 5);
        EXPECT_TRUE(std::distance(str.crbegin(), str.crend()) == 5);
        EXPECT_TRUE(std::distance(str.cbegin(), str.cbegin() + 5) == 5);
    }
}

TEST(StringNameTest, StringNameTest)
{
    {
        StringName name("atlas");
        EXPECT_TRUE(name.ToString() == "atlas");

        StringName name2 = name;
        EXPECT_TRUE(name2 == name);
    }
    {
        StringName name("atlas_12");
        EXPECT_TRUE(name.ToString() == "atlas_12" && name.GetNumber() == 12);
    }
    {
        StringName name("atlas_012");
        EXPECT_TRUE(name.ToString() == "atlas_012" && name.GetNumber() == 0);
    }
    {
        StringName name("_12");
        EXPECT_TRUE(name.IsNone() && name.ToString() == "none" && name.GetNumber() == 0);
    }
    {
        EXPECT_TRUE(StringName("") == StringName("_12"));
    }
#if !STRING_NAME_CASE_SENSITIVE
    {
        EXPECT_TRUE(StringName("atlas_12") == StringName("ATLAS_12"));
    };
#endif
    {
        EXPECT_TRUE(StringName("atlas_12").CompareLexical(StringName("BTLAS_12")) < 0);
    }
}

}
