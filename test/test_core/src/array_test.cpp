// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "container/array.hpp"
#include "boost/container/small_vector.hpp"

namespace atlas::test
{

struct PodStruct
{
    int32 integer;
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

TEST(ArrayAdd, ArrayTest)
{
    {
        Array<PodStruct> array;
        PodStruct elem{};
        EXPECT_TRUE(array.Add(elem) == 0 && array.Size() == 1 && array.Capacity() == 1);
        EXPECT_TRUE(array.Add({1}) == 1 && array.Size() == 2 && array.Capacity() == 2);

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

}
