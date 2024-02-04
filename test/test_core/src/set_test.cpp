// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "container/set.hpp"

namespace atlas::test
{

struct SetElement
{
    int32 integer;
    bool operator==(int32 right) const { return integer == right; }
    bool operator<(SetElement right) const { return integer < right.integer; }
};

TEST(SetCtor, SetTest)
{
    {
        Set<SetElement> set;
    }
    {
        Set<SetElement> set(10);
        EXPECT_TRUE(set.Capacity() == 10);
    }
    {
        Set<SetElement> set = {{4}, {1}, {3}, {3}};
        EXPECT_TRUE(std::is_sorted(set.begin(), set.end()) && set.Size() == 3);
    }
    {
        Array<SetElement> array = {{4}, {1}, {3}, {3}};
        Set<SetElement> set(array);
        EXPECT_TRUE(std::is_sorted(set.begin(), set.end()) && set.Size() == 3);
    }
};

TEST(SetInsert, SetTest)
{
    {
        Set<SetElement> set;
        bool already_in_set = false;
        set.Insert({1}, &already_in_set);
        EXPECT_TRUE(!already_in_set);
    }
    {
        Array<SetElement> pending_insert = {{4}, {1}, {3}, {3}};
        Set<SetElement> set;
        set.Insert({1});
        set.Insert(pending_insert);
        EXPECT_TRUE(std::is_sorted(set.begin(), set.end()) && set.Size() == 3);
    }
};

TEST(SetFind, SetTest)
{
    {
        Set<SetElement> set;
        set.Insert({1});
        EXPECT_TRUE(set.Contains({1}));
        auto it = set.Find({1});
        EXPECT_TRUE(*it == 1);
    }
};

TEST(SetRemove, SetTest)
{
    {
        Set<SetElement> set;
        set.Insert({1});
        EXPECT_TRUE(set.Remove({1}) && set.Size() == 0);
    }
};

TEST(MultiSetCtor, SetTest)
{
    {
        MultiSet<SetElement> set;
    }
    {
        MultiSet<SetElement> set(10);
        EXPECT_TRUE(set.Capacity() == 10);
    }
    {
        MultiSet<SetElement> set = {{4}, {1}, {3}, {3}};
        EXPECT_TRUE(std::is_sorted(set.begin(), set.end()) && set.Size() == 4);
    }
    {
        Array<SetElement> array = {{4}, {1}, {3}, {3}};
        MultiSet<SetElement> set(array);
        EXPECT_TRUE(std::is_sorted(set.begin(), set.end()) && set.Size() == 4);
    }
};

TEST(MultiSetInsert, SetTest)
{
    {
        MultiSet<SetElement> set;
        set.Insert({1});
        EXPECT_TRUE(set.Size() == 1);
        set.Insert({1});
        EXPECT_TRUE(set.Size() == 2);
    }
    {
        Array<SetElement> pending_insert = {{4}, {1}, {3}, {3}};
        MultiSet<SetElement> set;
        set.Insert({1});
        set.Insert(pending_insert);
        EXPECT_TRUE(std::is_sorted(set.begin(), set.end()) && set.Size() == 5);
    }
};

TEST(MultiSetFind, SetTest)
{
    {
        MultiSet<SetElement> set = {{4}, {1}, {3}, {3}};
        EXPECT_TRUE(set.Count({3}) == 2);
        EXPECT_TRUE(set.Count({5}) == 0);
    }
};

TEST(MultiSetRemove, SetTest)
{
    {
        MultiSet<SetElement> set = {{4}, {1}, {3}, {3}};
        EXPECT_TRUE(set.Remove({3}) == 2 && set.Size() == 2);
    }
};

}