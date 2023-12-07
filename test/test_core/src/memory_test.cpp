// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "memory/standard_malloc.hpp"
#include "memory/allocator.hpp"

namespace atlas::test
{

struct alignas(32) Alignment32
{
    bool flag;
};

TEST(StandardMallocTest, MemoryTest)
{
    StandardMalloc standard_malloc;

    void* ptr = standard_malloc.Malloc(100);
    EXPECT_TRUE(ptr != nullptr);

    ptr = standard_malloc.Realloc(ptr, 50);
    EXPECT_TRUE(ptr != nullptr);

    standard_malloc.Free(ptr);
    ptr = nullptr;

    ptr = standard_malloc.AlignedMalloc(64, 32);
    EXPECT_TRUE(ptr != nullptr && reinterpret_cast<uintptr_t>(ptr) % 32 == 0);

    ptr = standard_malloc.AlignedRealloc(ptr, 128, 32);
    EXPECT_TRUE(ptr != nullptr && reinterpret_cast<uintptr_t>(ptr) % 32 == 0);

    standard_malloc.AlignedFree(ptr);
}

TEST(OperatorNewDeleteTest, MemoryTest)
{
    // aligned new
    auto al = new Alignment32();
    EXPECT_TRUE(al != nullptr && reinterpret_cast<uintptr_t>(al) % 32 == 0);

    // aligned delete
    delete al;

    // aligned new[]
    auto al_array = new Alignment32[6]();
    EXPECT_TRUE(al_array != nullptr &&
        reinterpret_cast<uintptr_t>(al) % 32 == 0 &&
        reinterpret_cast<uintptr_t>(al_array + 1) % 32 == 0);

    // aligned delete[]
    delete[] al_array;

    // aligned no throw new
    auto al_nothrow = new(std::nothrow) Alignment32();
    EXPECT_TRUE(al_array != nullptr && reinterpret_cast<uintptr_t>(al) % 32 == 0);

    delete(al_nothrow);
}

TEST(StandardAllocatorTest, MemoryTest)
{
    using AllocType = StandardAllocator<int32>::Allocator<std::string>;
    using TraitsType = std::allocator_traits<AllocType>;
    static_assert(std::is_same_v<typename TraitsType::value_type, std::string>);
    static_assert(std::is_same_v<typename TraitsType::size_type , int32>);

    AllocType allocator;
    std::string* ptr = TraitsType::allocate(allocator, 100);
    EXPECT_TRUE(ptr != nullptr);
    TraitsType::deallocate(allocator, ptr, 100);
}

TEST(FixedAllocatorTest, MemoryTest)
{
    using AllocType = FixedAllocator<int32, 10>::Allocator<std::string>;
    using TraitsType = std::allocator_traits<AllocType>;
    static_assert(std::is_same_v<typename TraitsType::value_type, std::string>);
    static_assert(std::is_same_v<typename TraitsType::size_type , int32>);

    AllocType allocator;
    std::string* ptr = TraitsType::allocate(allocator, 2);
    EXPECT_TRUE(ptr != nullptr);
    EXPECT_TRUE(TraitsType::max_size(allocator) == 10);
    TraitsType::deallocate(allocator, ptr, 2);
}

TEST(InlineAllocatorTest, MemoryTest)
{
    using AllocType = InlineAllocator<int32, 10>::Allocator<std::string>;
    using TraitsType = std::allocator_traits<AllocType>;
    static_assert(std::is_same_v<typename TraitsType::value_type, std::string>);
    static_assert(std::is_same_v<typename TraitsType::size_type , int32>);

    AllocType allocator;
    std::string* ptr_0 = TraitsType::allocate(allocator, 2);
    EXPECT_TRUE(ptr_0 != nullptr);

    uintptr_t address_0 = reinterpret_cast<uintptr_t>(ptr_0);
    TraitsType::deallocate(allocator, ptr_0, 2);

    std::string* ptr_1 = TraitsType::allocate(allocator, 10);
    EXPECT_TRUE(ptr_1 != nullptr);

    uintptr_t address_1 = reinterpret_cast<uintptr_t>(ptr_1);
    TraitsType::deallocate(allocator, ptr_1, 10);
    EXPECT_TRUE(address_0 == address_1);

    std::string* ptr_2 = TraitsType::allocate(allocator, 11);
    EXPECT_TRUE(ptr_2 != nullptr);

    uintptr_t address_2 = reinterpret_cast<uintptr_t>(ptr_2);
    TraitsType::deallocate(allocator, ptr_2, 10);
    EXPECT_TRUE(address_0 != address_2);

}

}
