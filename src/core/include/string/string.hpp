// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>

#include "utility/compression_pair.hpp"
#include "memory/allocator.hpp"

namespace atlas
{
namespace details
{

template<typename CharTraits, typename SizeType>
class StringVal
{
    using CharType = CharTraits::char_type;
public:
    static constexpr size_t INLINE_SIZE = (sizeof(intptr_t) / sizeof(CharType) < 1) ? 1 : sizeof(intptr_t) / sizeof(CharType);

    bool LargeStringEngaged() const { return INLINE_SIZE < capacity_; }
    CharType* GetPtr() { return LargeStringEngaged() ? u_.ptr_ : u_.buffer_; }
    const CharType* GetPtr() const { return LargeStringEngaged() ? u_.ptr_ : u_.buffer_; }

    SizeType size_{ 0 };
    SizeType capacity_{ INLINE_SIZE };

    union
    {
        CharType* ptr_;
        CharType buffer_[INLINE_SIZE];
    } u_;
};

}

class CORE_API String
{
public:
    using ValueType = char8_t;
    using CharTraits = std::char_traits<ValueType>;
    using AllocatorType = StandardAllocator<int32>::Allocator<ValueType>;
    using AllocatorTraits = std::allocator_traits<AllocatorType>;
    using SizeType = AllocatorTraits::size_type;

private:
    using ValType = details::StringVal<CharTraits, SizeType>;
    using ParamType = typename CallTraits<ValueType>::ParamType;
    using Pointer = ValueType*;
    using ConstPointer = const ValueType*;

public:
    String();
    String(ParamType ch);
    String(ConstPointer str, SizeType len = -1);
    ~String();

    void Reserve(SizeType capacity);
protected:
    bool LargeStringEngaged() const { return GetVal().LargeStringEngaged(); }

    AllocatorType& GetAlloc() { return pair_.First(); }
    const AllocatorType& GetAlloc() const { return pair_.First(); }

    ValType& GetVal() { return pair_.Second(); }
    const ValType& GetVal() const { return pair_.Second(); }

    void BecomeLarge(SizeType capacity);

    void Init();


    CompressionPair<AllocatorType, ValType> pair_;
};

}
