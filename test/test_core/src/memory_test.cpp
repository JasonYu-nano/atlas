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

TEST(MemoryTest, StandardMallocTest)
{
    StandardMalloc standard_malloc;

    void* ptr = standard_malloc.malloc(100);
    EXPECT_TRUE(ptr != nullptr);

    ptr = standard_malloc.realloc(ptr, 50);
    EXPECT_TRUE(ptr != nullptr);

    standard_malloc.free(ptr);
    ptr = nullptr;

    ptr = standard_malloc.aligned_malloc(64, 32);
    EXPECT_TRUE(ptr != nullptr && reinterpret_cast<uintptr_t>(ptr) % 32 == 0);

    ptr = standard_malloc.aligned_realloc(ptr, 128, 32);
    EXPECT_TRUE(ptr != nullptr && reinterpret_cast<uintptr_t>(ptr) % 32 == 0);

    standard_malloc.aligned_free(ptr);
}

TEST(MemoryTest, OperatorNewDeleteTest)
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

TEST(MemoryTest, HeapAllocatorTest)
{
    using AllocType = HeapAllocator<std::string, int32>;
    using TraitsType = std::allocator_traits<AllocType>;
    static_assert(std::is_same_v<typename TraitsType::value_type, std::string>);
    static_assert(std::is_same_v<typename TraitsType::size_type , int32>);

    AllocType allocator;
    std::string* ptr = TraitsType::allocate(allocator, 100);
    EXPECT_TRUE(ptr != nullptr);
    TraitsType::deallocate(allocator, ptr, 100);
}

TEST(MemoryTest, StackAllocatorTest)
{
    using AllocType = StackAllocator<std::string, 10, int32>;
    using TraitsType = std::allocator_traits<AllocType>;
    static_assert(std::is_same_v<typename TraitsType::value_type, std::string>);
    static_assert(std::is_same_v<typename TraitsType::size_type , int32>);

    AllocType allocator;
    std::string* ptr = TraitsType::allocate(allocator, 2);
    EXPECT_TRUE(ptr != nullptr);
    EXPECT_TRUE(TraitsType::max_size(allocator) == 10);
    TraitsType::deallocate(allocator, ptr, 2);
}

TEST(MemoryTest, InlineAllocatorTest)
{
    using AllocType = InlineAllocator<std::string, 10, int32, HeapAllocator<void>>;
    using TraitsType = AllocatorTraits<AllocType>;
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

    std::string* ptr_3 = TraitsType::allocate(allocator, 2);
    std::string* ptr_4 = TraitsType::reallocate(allocator, ptr_3, 2, 10);
    EXPECT_TRUE(ptr_3 == ptr_4);
    std::string* ptr_5 = TraitsType::reallocate(allocator, ptr_4, 10, 11);
    EXPECT_TRUE(ptr_4 != ptr_5);
    std::string* ptr_6 = TraitsType::reallocate(allocator, ptr_5, 11, 15);
    std::string* ptr_7 = TraitsType::reallocate(allocator, ptr_6, 15, 2);
    TraitsType::deallocate(allocator, ptr_7, 2);

}

}
