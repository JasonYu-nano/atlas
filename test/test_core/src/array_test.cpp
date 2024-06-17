// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "container/array.hpp"

namespace atlas::test
{

struct PodStruct
{
    int32 integer;
    bool operator==(int32 right) const { return integer == right; }
};

struct NonPodStruct
{
    NonPodStruct() = default;
    NonPodStruct(const std::string& in_str) : str(in_str) {}
    std::string str;
};

TEST(ArrayTest, ArrayCtor)
{
    {
        Array<int32> array;
    }
    {
        Array<int32> array(10);
        EXPECT_TRUE(array.capacity() == 10);
    }
    {
        Array<int32> array = { 1, 2, 3 };
        EXPECT_TRUE(array.size() == 3);
    }
    {
        int a[3] = { 1, 2, 3 };
        Array<int32> array(a);
        EXPECT_TRUE(array.size() == 3);
    }
    {
        Array<int32> array(1, 10);
        EXPECT_TRUE(array.size() == 10);
    }
    {
        Array<int32> array_0(1, 10);
        Array<int32> array_1 = array_0;
        EXPECT_TRUE(array_1.size() == 10);
        Array<int32> array_2 = std::move(array_0);
        EXPECT_TRUE(array_1.size() == 10 && array_0.data() == nullptr);
    }
}

TEST(ArrayTest, ArrayAssign)
{
    {
        Array<PodStruct> array = { {1}, {2} };
        Array<PodStruct> array_2 = { {3}, {4}, {5} };
        array = array_2;
        EXPECT_TRUE(array.size() == 3 && array[2] == 5);
    }
    {
        Array<PodStruct> array = { {1}, {2} };
        Array<PodStruct> array_2 = { {3} };
        array = array_2;
        EXPECT_TRUE(array.size() == 1 && array[0] == 3);
    }
    {
        Array<PodStruct> array = { {1}, {2} };
        Array<PodStruct> array_2 = { {3}, 20 };
        array = array_2;
        EXPECT_TRUE(array.size() == 20 && array[0] == 3);
    }
    {
        Array<PodStruct> array = { {1}, {2} };
        Array<PodStruct> array_2 = { {3}, {4}, {5} };
        array = std::move(array_2);
        EXPECT_TRUE(array.size() == 3 && array[2] == 5 && array_2.size() == 0 && array_2.data() == nullptr);
    }
    {
        Array<PodStruct> array = { {1}, {2} };
        InlineArray<PodStruct, 10> array_2 = { {3}, {4}, {5} };
        array = std::move(array_2);
        EXPECT_TRUE(array.size() == 3 && array[2] == 5 && array_2.size() == 0);
    }
}

TEST(ArrayTest, ArrayAdd)
{
    {
        Array<PodStruct> array;
        PodStruct elem{};
        EXPECT_TRUE(array.add(elem) == 0 && array.size() == 1);
        EXPECT_TRUE(array.add({1}) == 1 && array.size() == 2);

        PodStruct elems[2] = {{2}, {3}};
        EXPECT_TRUE(array.append(elems) == 2 && array.size() == 4 && array.capacity() == 4);

        Array<PodStruct> array_2 = { {4}, {5} };
        array.append(array_2);
        EXPECT_TRUE(array.size() == 6 && array_2.size() == 2);

        array.append(std::move(array_2));
        EXPECT_TRUE(array.size() == 8 && array_2.size() == 0);
    }
    {
        Array<NonPodStruct> array;
        NonPodStruct elem{};
        EXPECT_TRUE(array.add(elem) == 0);
        EXPECT_TRUE(array.emplace("some string") == 1);

        NonPodStruct elems[2] = {{"string"}, {"string"}};
        EXPECT_TRUE(array.append(elems) == 2);

        Array<NonPodStruct> array_2 = { {"string"}, {"string"} };
        array.append(array_2);
        EXPECT_TRUE(array.size() == 6 && array_2.size() == 2);

        array.append(std::move(array_2));
        EXPECT_TRUE(array.size() == 8 && array_2.size() == 0);

        array.clear();
        EXPECT_TRUE(array.capacity() > 0);

        array.clear(true);
        EXPECT_TRUE(array.capacity() == 0);
    }
}

TEST(ArrayTest, ArrayFind)
{
    Array<int32> array = { 1, 2, 3, 4, 5 };
    EXPECT_TRUE(array.find(0) == INDEX_NONE);
    EXPECT_TRUE(array.find(2) == 1);

    EXPECT_TRUE(array.find([](auto&& elem) { return elem <= 0; }) == INDEX_NONE);
    EXPECT_TRUE(array.find([](auto&& elem) { return elem > 3; }) == 3);

    EXPECT_TRUE(array.find_last(0) == INDEX_NONE);
    EXPECT_TRUE(array.find_last(4) == 3);

    EXPECT_TRUE(array.find_last([](auto&& elem) { return elem <= 0; }) == INDEX_NONE);
    EXPECT_TRUE(array.find_last([](auto&& elem) { return elem > 3; }) == 4);
}

TEST(ArrayTest, ArrayInsert)
{
    {
        Array<NonPodStruct> array = { {"a"}, {"b"}, {"c"} };
        array.insert(array.cbegin(), {"move_able"});
        NonPodStruct copy_able = {"copy_able"};
        array.insert(array.cend(), copy_able);
        EXPECT_TRUE(array[0].str == "move_able");
        EXPECT_TRUE(array[4].str == "copy_able");
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        int32 insert[3] = {0, 0, 0};
        array.insert(array.begin() + 2, insert);
        EXPECT_TRUE(array[1] == 2 && array[2] == 0 && array[4] == 0 && array[5] == 3);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        Array<int32> insert = { 0, 0, 0 };
        array.insert(array.begin() + 2, insert);
        EXPECT_TRUE(array[1] == 2 && array[2] == 0 && array[4] == 0 && array[5] == 3);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        Array<int32> insert = { 0, 0, 0 };
        array.insert(array.begin() + 2, std::move(insert));
        EXPECT_TRUE(array[1] == 2 && array[2] == 0 && array[4] == 0 && array[5] == 3);
    }
}

TEST(ArrayTest, ArrayRemove)
{
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        auto ret = array.remove_at(1);
        EXPECT_TRUE(*ret == 3 && array.size() == 4);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        auto ret = array.remove_at(array.cbegin() + 1, 3);
        EXPECT_TRUE(*ret == 5 && array.size() == 2);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        array.remove(2);
        EXPECT_TRUE(array.size() == 4 && array[1] == 3);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        auto ret = array.remove_at_swap(array.cbegin() + 1);
        EXPECT_TRUE(*ret == 5 && array.size() == 4);
    }
    {
        Array<int32> array = { 1, 2, 3, 3, 5, 3, 6, 3 };
        auto ret = array.remove_all(3);
        EXPECT_TRUE(ret == 4 && array[2] == 5 && array[3] == 6);
    }
    {
        Array<int32> array = { 1, 2, 3, 3, 5, 3, 6, 3 };
        auto ret = array.remove_all_swap(3);
        EXPECT_TRUE(ret == 4 && array[2] == 6 && array[3] == 5);
    }
}

}
