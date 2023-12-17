// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "string/string.hpp"

namespace atlas::test
{

TEST(StringCtor, StringTest)
{
    {
        String str;
        str.Reserve(20);
        EXPECT_TRUE(str.Length() == 0);
        EXPECT_TRUE(str.Capacity() == 20);
    }

    {
        String str(u8'a');
        EXPECT_TRUE(str.Length() == 1);
        EXPECT_TRUE(str == String('a'));
    }

    {
        String str(u8'b', 64);
        EXPECT_TRUE(str.Length() == 64);
        EXPECT_TRUE(str == String('b', 64));
    }

    {
        auto raw_str = u8"this is an utf-8 string";
        String str(raw_str);
        EXPECT_TRUE(str.Length() == std::char_traits<char8_t>::length(raw_str));
        EXPECT_TRUE(str == String("this is an utf-8 string"));
    }

    {
        auto raw_str = u8"this is an utf-8 string";
        String str(raw_str, 16);
        EXPECT_TRUE(str.Length() == 16);
        EXPECT_TRUE(str == String("this is an utf-8"));
    }

    {
        String str(u8"💖");
        EXPECT_TRUE(str.Count() == 1 && str.Length() == 4);
    }

    {
        String str(u8"你好atlas");
        EXPECT_TRUE(str.Count() == 7 && str.Length() == 11);
    }

    {
        String str_0(u8"你好atlas");
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
        String str_0 = u8"你好atlas";
        String str_1 = u8"test";
        str_1 = str_0;
        EXPECT_TRUE(str_1 == str_0);

        str_1 = std::move(str_0);
        EXPECT_TRUE(str_1 == u8"你好atlas");

        str_1 = "";
        EXPECT_TRUE(str_1.Length() == 0);
    }
}

}
