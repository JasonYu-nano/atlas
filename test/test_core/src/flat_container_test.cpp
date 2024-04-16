// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "container/set.hpp"
#include "container/map.hpp"
#include "container/unordered_set.hpp"
#include "container/unordered_map.hpp"

namespace atlas::test
{

struct SetElement
{
    int32 integer;
    bool operator==(int32 right) const { return integer == right; }
    bool operator<(SetElement right) const { return integer < right.integer; }
};

TEST(SetTest, SetCtor)
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

TEST(SetTest, SetInsert)
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

TEST(SetTest, SetFind)
{
    {
        Set<SetElement> set;
        set.Insert({1});
        EXPECT_TRUE(set.Contains({1}));
        auto it = set.Find({1});
        EXPECT_TRUE(*it == 1);
    }
};

TEST(SetTest, SetRemove)
{
    {
        Set<SetElement> set;
        set.Insert({1});
        EXPECT_TRUE(set.Remove({1}) && set.Size() == 0);
    }
};

TEST(MultiSetTest, MultiSetCtor)
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

TEST(MultiSetTest, MultiSetInsert)
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

TEST(MultiSetTest, MultiSetFind)
{
    {
        MultiSet<SetElement> set = {{4}, {1}, {3}, {3}};
        EXPECT_TRUE(set.Count({3}) == 2);
        EXPECT_TRUE(set.Count({5}) == 0);
    }
};

TEST(MultiSetTest, MultiSetRemove)
{
    {
        MultiSet<SetElement> set = {{4}, {1}, {3}, {3}};
        EXPECT_TRUE(set.Remove({3}) == 2 && set.Size() == 2);
    }
};

TEST(MapTest, MapCtor)
{
    {
        Map<SetElement, int32> map;
    }
    {
        Map<SetElement, int32> map(10);
        EXPECT_TRUE(map.Capacity() == 10);
    }
    {
        Map<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        EXPECT_TRUE(std::is_sorted(map.begin(), map.end()) && map.Size() == 3);
    }
    {
        Array<std::pair<SetElement, int32>> array = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        Map<SetElement, int32> map(array);
        EXPECT_TRUE(std::is_sorted(map.begin(), map.end()) && map.Size() == 3);
    }
    {
        Map<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        Map<SetElement, int32> map_1(map);
        EXPECT_TRUE(map_1.Size() == 3);
        Map<SetElement, int32> map_2(std::move(map));
        EXPECT_TRUE(map_2.Size() == 3 && map.Size() == 0);
    }
};

TEST(MapTest, MapInsert)
{
    {
        Map<SetElement, int32> map;
        map.Insert({1}, 0);
        EXPECT_TRUE(map.FindValueRef({1}) == 0);
        Map<SetElement, int32> pending_insert = {{{4}, 0}, {{1}, 1}, {{3}, 0}};
        map.Insert(pending_insert);
        EXPECT_TRUE(map.Size() == 3 && map.FindValueRef({1}) == 0);
    }
    {
        Map<SetElement, int32> map;
        EXPECT_TRUE(map.FindOrInsert({1})->second == 0);
        map.Insert({2}, 5);
        EXPECT_TRUE(map.FindOrInsert({2})->second == 5);
    }
}

TEST(MapTest, MapRemove)
{
    {
        Map<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}};
        map.Remove({3});
        EXPECT_TRUE(map.Size() == 2 && map.FindValue({3}) == nullptr);
    }
};

TEST(MultiMapTest, MultiMapCtor)
{
    {
        MultiMap<SetElement, int32> map;
    }
    {
        MultiMap<SetElement, int32> map(10);
        EXPECT_TRUE(map.Capacity() == 10);
    }
    {
        MultiMap<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        EXPECT_TRUE(std::is_sorted(map.begin(), map.end()) && map.Size() == 4);
    }
    {
        Array<std::pair<SetElement, int32>> array = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        MultiMap<SetElement, int32> map(array);
        EXPECT_TRUE(std::is_sorted(map.begin(), map.end()) && map.Size() == 4);
    }
    {
        MultiMap<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        MultiMap<SetElement, int32> map_1(map);
        EXPECT_TRUE(map_1.Size() == 4);
        MultiMap<SetElement, int32> map_2(std::move(map));
        EXPECT_TRUE(map_2.Size() == 4 && map.Size() == 0);
    }
};

TEST(MultiMapTest, MultiMapInsert)
{
    {
        MultiMap<SetElement, int32> map;
        map.Insert({1}, 0);
        EXPECT_TRUE(map.FindValueRef({1}) == 0);
        MultiMap<SetElement, int32> pending_insert = {{{4}, 0}, {{1}, 1}, {{3}, 0}};
        map.Insert(pending_insert);
        EXPECT_TRUE(map.Count({1}) == 2 && map.Size() == 4 && map.FindValueRef({1}) == 0);
    }
    {
        MultiMap<SetElement, int32> map;
        EXPECT_TRUE(map.FindOrInsert({1})->second == 0);
        map.Insert({2}, 5);
        EXPECT_TRUE(map.FindOrInsert({2})->second == 5);
    }
}

TEST(MultiMapTest, MultiMapRemove)
{
    {
        MultiMap<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 1}};
        map.Remove({3});
        EXPECT_TRUE(map.Size() == 2 && map.FindValue({3}) == nullptr);
    }
};

TEST(UnorderedSetTest, UnorderedSetCtor)
{
    UnorderedSet<int32> set;
    set.Insert(5);
    EXPECT_TRUE(set.Contains(5));
}

TEST(UnorderedMapTest, UnorderedMapCtor)
{
    UnorderedMap<std::string, int32> map;
    map.Insert("atlas", 5);
    EXPECT_TRUE(map.Contains("atlas"));
}

}