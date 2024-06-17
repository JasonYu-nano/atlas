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
        EXPECT_TRUE(map.capacity() == 10);
    }
    {
        Map<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        EXPECT_TRUE(std::is_sorted(map.begin(), map.end()) && map.size() == 3);
    }
    {
        Array<std::pair<SetElement, int32>> array = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        Map<SetElement, int32> map(array);
        EXPECT_TRUE(std::is_sorted(map.begin(), map.end()) && map.size() == 3);
    }
    {
        Map<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        Map<SetElement, int32> map_1(map);
        EXPECT_TRUE(map_1.size() == 3);
        Map<SetElement, int32> map_2(std::move(map));
        EXPECT_TRUE(map_2.size() == 3 && map.size() == 0);
    }
};

TEST(MapTest, MapInsert)
{
    {
        Map<SetElement, int32> map;
        map.insert({1}, 0);
        EXPECT_TRUE(map.find_value_ref({1}) == 0);
        Map<SetElement, int32> pending_insert = {{{4}, 0}, {{1}, 1}, {{3}, 0}};
        map.insert(pending_insert);
        EXPECT_TRUE(map.size() == 3 && map.find_value_ref({1}) == 0);
    }
    {
        Map<SetElement, int32> map;
        EXPECT_TRUE(map.find_or_insert({1})->second == 0);
        map.insert({2}, 5);
        EXPECT_TRUE(map.find_or_insert({2})->second == 5);
    }
}

TEST(MapTest, MapRemove)
{
    {
        Map<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}};
        map.remove({3});
        EXPECT_TRUE(map.size() == 2 && map.find_value({3}) == nullptr);
    }
};

TEST(MultiMapTest, MultiMapCtor)
{
    {
        MultiMap<SetElement, int32> map;
    }
    {
        MultiMap<SetElement, int32> map(10);
        EXPECT_TRUE(map.capacity() == 10);
    }
    {
        MultiMap<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        EXPECT_TRUE(std::is_sorted(map.begin(), map.end()) && map.size() == 4);
    }
    {
        Array<std::pair<SetElement, int32>> array = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        MultiMap<SetElement, int32> map(array);
        EXPECT_TRUE(std::is_sorted(map.begin(), map.end()) && map.size() == 4);
    }
    {
        MultiMap<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 5}};
        MultiMap<SetElement, int32> map_1(map);
        EXPECT_TRUE(map_1.size() == 4);
        MultiMap<SetElement, int32> map_2(std::move(map));
        EXPECT_TRUE(map_2.size() == 4 && map.size() == 0);
    }
};

TEST(MultiMapTest, MultiMapInsert)
{
    {
        MultiMap<SetElement, int32> map;
        map.insert({1}, 0);
        EXPECT_TRUE(map.find_value_ref({1}) == 0);
        MultiMap<SetElement, int32> pending_insert = {{{4}, 0}, {{1}, 1}, {{3}, 0}};
        map.insert(pending_insert);
        EXPECT_TRUE(map.count({1}) == 2 && map.size() == 4 && map.find_value_ref({1}) == 0);
    }
    {
        MultiMap<SetElement, int32> map;
        EXPECT_TRUE(map.find_or_insert({1})->second == 0);
        map.insert({2}, 5);
        EXPECT_TRUE(map.find_or_insert({2})->second == 5);
    }
}

TEST(MultiMapTest, MultiMapRemove)
{
    {
        MultiMap<SetElement, int32> map = {{{4}, 0}, {{1}, 1}, {{3}, 0}, {{3}, 1}};
        map.remove({3});
        EXPECT_TRUE(map.size() == 2 && map.find_value({3}) == nullptr);
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