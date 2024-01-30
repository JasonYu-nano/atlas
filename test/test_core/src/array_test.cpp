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

TEST(ArrayCtor, ArrayTest)
{
    {
        Array<int32> array;
    }
    {
        Array<int32> array(10);
        EXPECT_TRUE(array.Capacity() == 10);
    }
    {
        Array<int32> array = { 1, 2, 3 };
        EXPECT_TRUE(array.Size() == 3);
    }
    {
        int a[3] = { 1, 2, 3 };
        Array<int32> array(a);
        EXPECT_TRUE(array.Size() == 3);
    }
    {
        Array<int32> array(1, 10);
        EXPECT_TRUE(array.Size() == 10);
    }
    {
        Array<int32> array_0(1, 10);
        Array<int32> array_1 = array_0;
        EXPECT_TRUE(array_1.Size() == 10);
        Array<int32> array_2 = std::move(array_0);
        EXPECT_TRUE(array_1.Size() == 10 && array_0.Data() == nullptr);
    }
}

TEST(ArrayAssign, ArrayTest)
{
    {
        Array<PodStruct> array = { {1}, {2} };
        Array<PodStruct> array_2 = { {3}, {4}, {5} };
        array = array_2;
        EXPECT_TRUE(array.Size() == 3 && array[2] == 5);
    }
    {
        Array<PodStruct> array = { {1}, {2} };
        Array<PodStruct> array_2 = { {3} };
        array = array_2;
        EXPECT_TRUE(array.Size() == 1 && array[0] == 3);
    }
    {
        Array<PodStruct> array = { {1}, {2} };
        Array<PodStruct> array_2 = { {3}, 20 };
        array = array_2;
        EXPECT_TRUE(array.Size() == 20 && array[0] == 3);
    }
    {
        Array<PodStruct> array = { {1}, {2} };
        Array<PodStruct> array_2 = { {3}, {4}, {5} };
        array = std::move(array_2);
        EXPECT_TRUE(array.Size() == 3 && array[2] == 5 && array_2.Size() == 0 && array_2.Data() == nullptr);
    }
    {
        Array<PodStruct> array = { {1}, {2} };
        InlineArray<PodStruct, 10> array_2 = { {3}, {4}, {5} };
        array = std::move(array_2);
        EXPECT_TRUE(array.Size() == 3 && array[2] == 5 && array_2.Size() == 3);
    }
}

TEST(ArrayAdd, ArrayTest)
{
    {
        Array<PodStruct> array;
        PodStruct elem{};
        EXPECT_TRUE(array.Add(elem) == 0 && array.Size() == 1);
        EXPECT_TRUE(array.Add({1}) == 1 && array.Size() == 2);

        PodStruct elems[2] = {{2}, {3}};
        EXPECT_TRUE(array.Append(elems) == 2 && array.Size() == 4 && array.Capacity() == 4);

        Array<PodStruct> array_2 = { {4}, {5} };
        array.Append(array_2);
        EXPECT_TRUE(array.Size() == 6 && array_2.Size() == 2);

        array.Append(std::move(array_2));
        EXPECT_TRUE(array.Size() == 8 && array_2.Size() == 0);
    }
    {
        Array<NonPodStruct> array;
        NonPodStruct elem{};
        EXPECT_TRUE(array.Add(elem) == 0);
        EXPECT_TRUE(array.Emplace("some string") == 1);

        NonPodStruct elems[2] = {{"string"}, {"string"}};
        EXPECT_TRUE(array.Append(elems) == 2);

        Array<NonPodStruct> array_2 = { {"string"}, {"string"} };
        array.Append(array_2);
        EXPECT_TRUE(array.Size() == 6 && array_2.Size() == 2);

        array.Append(std::move(array_2));
        EXPECT_TRUE(array.Size() == 8 && array_2.Size() == 0);

        array.Clear();
        EXPECT_TRUE(array.Capacity() > 0);

        array.Clear(true);
        EXPECT_TRUE(array.Capacity() == 0);
    }
}

TEST(ArrayFind, ArrayTest)
{
    Array<int32> array = { 1, 2, 3, 4, 5 };
    EXPECT_TRUE(array.Find(0) == INDEX_NONE);
    EXPECT_TRUE(array.Find(2) == 1);

    EXPECT_TRUE(array.Find([](auto&& elem) { return elem <= 0; }) == INDEX_NONE);
    EXPECT_TRUE(array.Find([](auto&& elem) { return elem > 3; }) == 3);

    EXPECT_TRUE(array.FindLast(0) == INDEX_NONE);
    EXPECT_TRUE(array.FindLast(4) == 3);

    EXPECT_TRUE(array.FindLast([](auto&& elem) { return elem <= 0; }) == INDEX_NONE);
    EXPECT_TRUE(array.FindLast([](auto&& elem) { return elem > 3; }) == 4);
}

TEST(ArrayInsert, ArrayTest)
{
    {
        Array<NonPodStruct> array = { {"a"}, {"b"}, {"c"} };
        array.Insert(array.cbegin(), {"move_able"});
        NonPodStruct copy_able = {"copy_able"};
        array.Insert(array.cend(), copy_able);
        EXPECT_TRUE(array[0].str == "move_able");
        EXPECT_TRUE(array[4].str == "copy_able");
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        int32 insert[3] = {0, 0, 0};
        array.Insert(array.begin() + 2, insert);
        EXPECT_TRUE(array[1] == 2 && array[2] == 0 && array[4] == 0 && array[5] == 3);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        Array<int32> insert = { 0, 0, 0 };
        array.Insert(array.begin() + 2, insert);
        EXPECT_TRUE(array[1] == 2 && array[2] == 0 && array[4] == 0 && array[5] == 3);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        Array<int32> insert = { 0, 0, 0 };
        array.Insert(array.begin() + 2, std::move(insert));
        EXPECT_TRUE(array[1] == 2 && array[2] == 0 && array[4] == 0 && array[5] == 3);
    }
}

TEST(ArrayRemove, ArrayTest)
{
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        auto ret = array.RemoveAt(1);
        EXPECT_TRUE(*ret == 3 && array.Size() == 4);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        auto ret = array.RemoveAt(array.cbegin() + 1, 3);
        EXPECT_TRUE(*ret == 5 && array.Size() == 2);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        array.Remove(2);
        EXPECT_TRUE(array.Size() == 4 && array[1] == 3);
    }
    {
        Array<int32> array = { 1, 2, 3, 4, 5 };
        auto ret = array.RemoveAtSwap(array.cbegin() + 1);
        EXPECT_TRUE(*ret == 5 && array.Size() == 4);
    }
    {
        Array<int32> array = { 1, 2, 3, 3, 5, 3, 6, 3 };
        auto ret = array.RemoveAll(3);
        EXPECT_TRUE(ret == 4 && array[2] == 5 && array[3] == 6);
    }
    {
        Array<int32> array = { 1, 2, 3, 3, 5, 3, 6, 3 };
        auto ret = array.RemoveAllSwap(3);
        EXPECT_TRUE(ret == 4 && array[2] == 6 && array[3] == 5);
    }
}

}
