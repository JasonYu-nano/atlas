// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "string/string.hpp"

#include "unicode/utf8.h"

#include "math/atlas_math.hpp"

namespace atlas
{

String::String()
{
    TidyInit();
}

String::String(const char8_t ch)
{
    Construct(&ch, 1);
}

String::String(const char ch)
{
    auto u8 = static_cast<ValueType>(ch);
    Construct(&u8, 1);
}

String::String(const char8_t ch, String::SizeType count)
{
    Construct(ch, count);
}

String::String(const char ch, String::SizeType count)
{
    Construct(static_cast<ValueType>(ch), count);
}

String::String(const char8_t* str, String::SizeType len)
{
    if (len < 0)
    {
        size_t str_len = CharTraits::length(str);
        ASSERT(str_len <= std::numeric_limits<SizeType>::max());
        len = static_cast<SizeType>(str_len);
    }
    Construct(str, len);
}

String::String(const char* str, String::SizeType len)
{
    if (len < 0)
    {
        size_t str_len = std::char_traits<char>::length(str);
        ASSERT(str_len <= std::numeric_limits<SizeType>::max());
        len = static_cast<SizeType>(str_len);
    }
    Construct(reinterpret_cast<const char8_t*>(str), len);
}

String::String(const String& right)
{
    Construct(right, 0, std::numeric_limits<SizeType>::max());
}

String::String(const String& right, SizeType offset, SizeType size)
{
    Construct(right, offset, size);
}

String::String(String&& right) noexcept
{
    MoveConstruct(right, 0, std::numeric_limits<SizeType>::max());
}

String::String(String&& right, SizeType offset, SizeType size) noexcept
{
    MoveConstruct(right, offset, size);
}

String::~String()
{
    auto&& my_val = GetVal();
    if (LargeStringEngaged())
    {
        AllocatorTraits::deallocate(GetAlloc(), my_val.GetPtr(), my_val.capacity_ + 1);
    }
    my_val.size_ = 0;
    my_val.capacity_ = ValType::INLINE_SIZE - 1;
}

String& String::operator= (const String& right)
{
    Assign(right.Data(), right.Length());
    return *this;
}

String& String::operator= (String&& right) noexcept
{
    MoveAssign(right);
    return *this;
}

String& String::operator=(const char* right)
{
    Assign(reinterpret_cast<const char8_t*>(right), static_cast<SizeType>(std::char_traits<char>::length(right)));
    return *this;
}

String& String::operator=(const char8_t* right)
{
    Assign(right, static_cast<SizeType>(CharTraits::length(right)));
    return *this;
}

bool String::operator==(const String& right) const
{
    return Equals(right, ECaseSensitive::Sensitive);
}
bool String::operator!=(const String& right) const
{
    return !Equals(right, ECaseSensitive::Sensitive);
}

String::SizeType String::Count() const
{
    SizeType length = Length();
    SizeType count = 0;
    const char8_t* data = Data();
    UChar32 c;

    for (SizeType i = 0; i < length;)
    {
        U8_NEXT(data, i, length, c);
        if(c >= 0)
        {
            ++count;
        }
    }
    return count;
}

bool String::Equals(const String &right, ECaseSensitive case_sensitive) const
{
    if (case_sensitive == ECaseSensitive::Sensitive)
    {
        return Length() == right.Length() && CharTraits::compare(Data(), right.Data(), Length()) == 0;
    }
    //TODO: not implement!
    return false;
}

void String::Reserve(String::SizeType capacity)
{
    auto&& my_val = GetVal();
    if (capacity > my_val.capacity_)
    {
        if (!LargeStringEngaged())
        {
            BecomeLarge(capacity);
        }
        else
        {
            Pointer new_ptr = AllocatorTraits::allocate(GetAlloc(), capacity + 1);
            Pointer old_ptr = my_val.u_.ptr_;
            CharTraits::move(new_ptr, old_ptr, my_val.size_ + 1);
            AllocatorTraits::deallocate(GetAlloc(), old_ptr, my_val.capacity_ + 1);
            my_val.u_.ptr_ = new_ptr;
            my_val.capacity_ = capacity;
        }
    }
}

void String::BecomeLarge(String::SizeType capacity)
{
    auto&& my_val = GetVal();
    Pointer new_ptr = AllocatorTraits::allocate(GetAlloc(), capacity + 1);
    CharTraits::move(new_ptr, my_val.u_.buffer_, my_val.size_ + 1);
    my_val.u_.ptr_ = new_ptr;
    my_val.capacity_ = capacity;
}

void String::Construct(String::ConstPointer str, AllocatorTraits::size_type len)
{
    Reserve(len + 1);
    auto&& my_val = GetVal();
    Pointer ptr = my_val.GetPtr();
    CharTraits::copy(ptr, str, len);
    Eos(len);
}

void String::Construct(const String::ValueType ch, String::SizeType count)
{
    ASSERT(count > 0 && count < std::numeric_limits<SizeType>::max());
    Reserve(count + 1);
    auto&& my_val = GetVal();
    auto ptr = my_val.GetPtr();
    CharTraits::assign(ptr, count, ch);
    Eos(count);
}

void String::Construct(const String& right, SizeType offset, SizeType size)
{
    ASSERT(right.IsValidIndex(offset));
    SizeType actual_size = math::Clamp(size, 0, right.Length() - offset);
    if (actual_size > 0)
    {
        auto right_ptr = right.Data() + offset;
        Reserve(actual_size);
        CharTraits::copy(Data(), right_ptr, actual_size);
    }
    Eos(actual_size);
}

void String::MoveConstruct(String& right, String::SizeType offset, String::SizeType size)
{
    ASSERT(right.IsValidIndex(offset));
    SizeType actual_size = math::Clamp(size, 0, right.Length() - offset);
    if (actual_size > 0)
    {
        auto right_ptr = right.Data();
        if (offset > 0)
        {
            CharTraits::move(right_ptr, right_ptr + offset, actual_size);
            right.Eos(actual_size);
        }

        Reserve(actual_size);
        CharTraits::move(Data(), right_ptr, actual_size);
    }
    Eos(actual_size);
}

void String::Assign(const char8_t* right, SizeType length)
{
    ASSERT(IsValidAddress(right, right + length));
    Reserve(length);
    CharTraits::copy(Data(), right, length);
    Eos(length);
}

void String::MoveAssign(String& right)
{
    ASSERT(IsValidAddress(right.Data(), right.Data() + right.Length()));
    SizeType size = right.Length();
    Reserve(size);
    CharTraits::move(Data(), right.Data(), size);
    Eos(size);
}

void String::Eos(String::SizeType size)
{
    auto&& my_val = GetVal();
    my_val.size_ = size;
    CharTraits::assign(my_val.GetPtr()[size], ValueType());
}

bool String::IsValidAddress(const char8_t* start, const char8_t* end) const
{
    auto my_start = Data();
    auto my_end = my_start + Length();
    return (start < my_start || start > my_end) && (end < my_start || end > my_end);
}

}
