// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "string/string.hpp"
#include "string/string_name.hpp"

namespace atlas::test
{

TEST(StringCtor, StringTest)
{
    {
        String str;
        str.Reserve(20);
        EXPECT_TRUE(str.Length() == 0);
        EXPECT_TRUE(str.Capacity() == 22);
    }

    {
        String str('a');
        EXPECT_TRUE(str.Length() == 1);
        EXPECT_TRUE(str == String('a'));
    }

    {
        String str('b', 64);
        EXPECT_TRUE(str.Length() == 64);
        EXPECT_TRUE(str == String('b', 64));
    }

    {
        auto raw_str = "this is an utf-8 string";
        String str(raw_str);
        EXPECT_TRUE(str.Length() == std::char_traits<char>::length(raw_str));
        EXPECT_TRUE(str == String("this is an utf-8 string"));
    }

    {
        auto raw_str = "this is an utf-8 string";
        String str(raw_str, 16);
        EXPECT_TRUE(str.Length() == 16);
        EXPECT_TRUE(str == String("this is an utf-8"));
    }

    {
        String str("💖");
        EXPECT_TRUE(str.Count() == 1 && str.Length() == 4);
    }

    {
        String str("你好atlas");
        EXPECT_TRUE(str.Count() == 7 && str.Length() == 11);
    }

    {
        String str_0("你好atlas");
        String str_1(std::move(str_0));
        EXPECT_TRUE(str_1.Length() == 11);

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
        EXPECT_TRUE(str_1.Length() == 0);
    }
}

TEST(StringFormat, StringTest)
{
    {
        String name = "atlas";
        String str = String::Format("my name is {0}", name);
        EXPECT_TRUE(str == "my name is atlas");
    }

    {
        String str = String::Format("my id is {0}", 1);
        EXPECT_TRUE(str == "my id is 1");
    }

    {
        String name = "阿特拉斯";
        String str = String::Format("my name is {0}", name);
        EXPECT_TRUE(str == "my name is 阿特拉斯");
    }
}

TEST(StringAt, StringTest)
{
    {
        String name = "阿特拉斯";
        EXPECT_TRUE(String::char_traits::to_int_type(name[3]) == 231);
        EXPECT_TRUE(name.CodePointAt(1) == 29305);
        EXPECT_TRUE(name.CodePointAt(4) == CodePoint::incomplete);
    }
}

TEST(StringConvert, StringTest)
{
    {
        String name = String::FromUtf16(u"阿特拉斯");
        EXPECT_TRUE(name == "阿特拉斯");
    }

    {
        String name = String::FromUtf32(U"阿特拉斯");
        EXPECT_TRUE(name == "阿特拉斯");
    }

    {
        String name = String::From(std::wstring(L"阿特拉斯"));
        EXPECT_TRUE(name == "阿特拉斯");
    }

    {
        String name = "Atlas";
        EXPECT_TRUE(name.ToLower() == "atlas");
        EXPECT_TRUE(name.ToLower().IsLower());
    }

    {
        String name = "atlas";
        EXPECT_TRUE(name.ToUpper() == "ATLAS");
        EXPECT_TRUE(name.ToUpper().IsUpper());
    }
}

TEST(StringEquals, StringTest)
{
    {
        String name = "Atlas";
        EXPECT_FALSE(name.Equals("atlas"));
        EXPECT_TRUE(name.Equals("atlas", ECaseSensitive::Insensitive));
    }
}

TEST(StringSearch, StringTest)
{
    {
        String name = "Atlas阿特拉斯";
        EXPECT_FALSE(name.StartsWith(String("atlas")));
        EXPECT_TRUE(name.StartsWith("Atlas"));
        EXPECT_FALSE(name.StartsWith(String("Atlas阿特拉斯A")));
        EXPECT_TRUE(name.StartsWith("atlas", ECaseSensitive::Insensitive));
    }
    {
        String name = "阿特拉斯Atlas";
        EXPECT_FALSE(name.EndsWith(String("atlas")));
        EXPECT_TRUE(name.EndsWith("Atlas"));
        EXPECT_FALSE(name.EndsWith(String("阿特拉斯AtlasA")));
        EXPECT_TRUE(name.EndsWith("atlas", ECaseSensitive::Insensitive));
    }
    {
        String name = "Atlas";
        EXPECT_TRUE(name.IndexOf("la") == 2);
        EXPECT_TRUE(name.IndexOf("al") == INDEX_NONE);
        EXPECT_TRUE(name.IndexOf("at", ECaseSensitive::Insensitive) == 0);
        EXPECT_TRUE(name.IndexOf(String::view_type(CHAR_T("at"))) == INDEX_NONE);
        std::shared_ptr<String> a;
    }
}

TEST(StringModify, StringTest)
{
    {
        String name = "Atlas";
        String::view_type view = CHAR_T("阿特拉斯");
        name.Append(view);
        EXPECT_TRUE(name == "Atlas阿特拉斯" && name.Length() == 17);
    }
    {
        String name = "Atlas";
        name.Prepend(String("阿特拉斯"));
        EXPECT_TRUE(name == "阿特拉斯Atlas" && name.Length() == 17);
    }
    {
        String name = "Atlas";
        String new_name = name.Concat("阿特拉斯");
        EXPECT_TRUE(name == "Atlas" && new_name == "Atlas阿特拉斯" && new_name.Length() == 17);
    }
    {
        String name = "Atlas";
        name.Insert(5, "阿特拉斯");
        EXPECT_TRUE(name == "Atlas阿特拉斯" && name.Length() == 17);
    }
    {
        String name = "Atlas";
        name.Insert(0, "阿特拉斯");
        EXPECT_TRUE(name == "阿特拉斯Atlas" && name.Length() == 17);
    }
    {
        String name = "Atlas";
        name.Remove(2, 2);
        EXPECT_TRUE(name == "Ats" && name.Length() == 3);
    }
    {
        String name = "this is an utf-8 string";
        name.Remove("is");
        EXPECT_TRUE(name == "th  an utf-8 string");
    }
    {
        String str = "this is a string";
        str.Replace(8, 1, "an utf-8");
        EXPECT_TRUE(str == "this is an utf-8 string");
    }
    {
        String str = "cbegin is the const begin iterator of the string";
        str.Replace("begin", "end");
        EXPECT_TRUE(str == "cend is the const end iterator of the string");
    }

}

TEST(StringIter, StringTest)
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
