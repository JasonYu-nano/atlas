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
    static constexpr size_t INLINE_SIZE = (16 / sizeof(CharType) < 1) ? 1 : 16 / sizeof(CharType);

    bool LargeStringEngaged() const { return INLINE_SIZE <= capacity_; }
    CharType* GetPtr() { return LargeStringEngaged() ? u_.ptr_ : u_.buffer_; }
    const CharType* GetPtr() const { return LargeStringEngaged() ? u_.ptr_ : u_.buffer_; }

    SizeType size_{ 0 };
    SizeType capacity_{ INLINE_SIZE - 1 };

    union
    {
        CharType buffer_[INLINE_SIZE];
        CharType* ptr_;
    } u_;
};

}

enum class ECaseSensitive : uint8
{
    Sensitive,
    Insensitive
};

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
    explicit String(char8_t ch);
    explicit String(char ch);

    String(char8_t ch, SizeType count);
    String(char ch, SizeType count);

    String(const char8_t* str, SizeType len = -1);
    String(const char* str, SizeType len = -1);

    String(const String& right);
    String(const String& right, SizeType offset, SizeType size = std::numeric_limits<SizeType>::max());
    String(String&& right) noexcept;
    String(String&& right, SizeType offset, SizeType size = std::numeric_limits<SizeType>::max()) noexcept;

    ~String();

    String& operator= (const String& right);
    String& operator= (String&& right) noexcept;
    String& operator= (const char* right);
    String& operator= (const char8_t* right);

    bool operator== (const String& right) const;
    bool operator!= (const String& right) const;

    NODISCARD inline char8_t* Data();
    NODISCARD inline const char8_t* Data() const;

    SizeType Size() const;

    SizeType Length() const;

    NODISCARD SizeType Count() const;

    SizeType Capacity() const;

    NODISCARD bool Equals(const String& right, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    void Reserve(SizeType capacity);

    NODISCARD bool IsValidIndex(SizeType index) const;
protected:
    bool LargeStringEngaged() const { return GetVal().LargeStringEngaged(); }

    AllocatorType& GetAlloc() { return pair_.First(); }
    const AllocatorType& GetAlloc() const { return pair_.First(); }

    ValType& GetVal() { return pair_.Second(); }
    const ValType& GetVal() const { return pair_.Second(); }

    void BecomeLarge(SizeType capacity);

    void TidyInit();

    void Construct(ConstPointer str, SizeType len);
    void Construct(char8_t ch, SizeType count);
    void Construct(const String& right, SizeType offset, SizeType size);

    void MoveConstruct(String& right, SizeType offset, SizeType size);

    void Assign(const char8_t* right, SizeType length);

    void MoveAssign(String& right);

    void Eos(SizeType size);

    bool IsValidAddress(const char8_t* start, const char8_t* end) const;

    CompressionPair<AllocatorType, ValType> pair_;
};

inline char8_t* String::Data()
{
    return GetVal().GetPtr();
}

inline const char8_t* String::Data() const
{
    return GetVal().GetPtr();
}

inline String::SizeType String::Size() const
{
    return GetVal().size_ + 1;
}

inline String::SizeType String::Length() const
{
    return GetVal().size_;
}

inline String::SizeType String::Capacity() const
{
    return GetVal().capacity_;
}

inline bool String::IsValidIndex(String::SizeType index) const
{
    return index >= 0 && index < Length();
}

inline void String::TidyInit()
{
    Eos(0);
}

}
