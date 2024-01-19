// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "string/string.hpp"
#include "math/atlas_math.hpp"

namespace atlas
{

namespace
{
    int32 ICompare(String::const_pointer lhs, String::const_pointer rhs, String::size_type length)
    {
        auto&& fold_case = details::FoldCaseUnsafe<String::value_type>();
        auto&& loc = locale::DefaultLocale();
        for (String::size_type i = 0; i < length; ++i)
        {
            int32 diff = fold_case(*(lhs + i), loc) - fold_case(*(rhs + i), loc);
            if (diff != 0)
            {
                return diff;
            }
        }
        return 0;
    }
}

String::size_type String::Count() const
{
    auto it = reinterpret_cast<const char*>(Data());
    auto end = it + Length();
    size_type count = 0;

    while (it < end)
    {
        int32 next = UtfTraits<char>::trail_length(*it) + 1;
        it += next;
        ++count;
    }

    return count;
}

CodePoint String::CodePointAt(std::make_unsigned_t<size_type> offset) const
{
    auto it = reinterpret_cast<const char*>(Data());
    auto end = it + Length();
    size_type current_offset = 0;

    while (it < end)
    {
        if (current_offset == offset)
        {
            return UtfTraits<char>::decode(it, end);
        }

        int32 next = UtfTraits<char>::trail_length(*it) + 1;
        it += next;
        ++current_offset;
    }

    return CodePoint::incomplete;
}

int32 String::Compare(const String& right, ECaseSensitive case_sensitive) const
{
    const size_type left_length = Length();
    const size_type right_length = right.Length();

    if (left_length != right_length)
    {
        return left_length < right_length ? -1 : 1;
    }

    if (case_sensitive == ECaseSensitive::Sensitive)
    {
        return char_traits::compare(Data(), right.Data(), left_length);
    }

    return ICompare(Data(), right.Data(), left_length);
}

String String::FoldCase() const
{
    const char* data = reinterpret_cast<const char*>(Data());
    std::string fold_case = boost::locale::fold_case(data, data + Length(), locale::DefaultLocale());
    return {fold_case.data(), ConvertSize(fold_case.length())};
}

bool String::IsUpper(const std::locale& locale) const
{
    return ToUpper(locale) == *this;
}

String String::ToUpper(const std::locale& locale) const
{
    const char* data = reinterpret_cast<const char*>(Data());
    std::string upper = boost::locale::to_upper(data, data + Length(), locale);
    return {upper.data(), ConvertSize(upper.length())};
}

bool String::IsLower(const std::locale& locale) const
{
    return ToLower(locale) == *this;
}

String String::ToLower(const std::locale& locale) const
{
    const char* data = reinterpret_cast<const char*>(Data());
    std::string lower = boost::locale::to_lower(data, data + Length(), locale);
    return {lower.data(), ConvertSize(lower.length())};
}

String& String::Remove(size_type from, size_type count)
{
    ASSERT(count > 0 && from < Length() && from + count <= Length());

    pointer start = Data() + from;
    size_type length = Length();
    size_type new_length = length - count;
    char_traits::move(start, start + count, length - from - count);
    Eos(new_length);

    return *this;
}

String String::FromUtf16(const char16_t* str, size_type length)
{
    if (length <= 0)
    {
        return {};
    }

    std::string u8 = boost::locale::conv::utf_to_utf<char, char16_t>(str, str + length);
    return {u8.data(), ConvertSize(u8.length())};
}

String String::FromUtf32(const char32_t* str, size_type length)
{
    if (length <= 0)
    {
        return {};
    }

    std::string u8 = boost::locale::conv::utf_to_utf<char, char32_t>(str, str + length);
    return {u8.data(), ConvertSize(u8.length())};
}

void String::Construct(String::const_pointer str, size_type len)
{
    ASSERT(len < std::numeric_limits<size_type>::max());
    auto&& my_val = GetVal();
    if (len > my_val.capacity_)
    {
        Reserve(len);
    }
    pointer ptr = my_val.GetPtr();
    char_traits::move(ptr, str, len);
    Eos(len);
}

void String::Construct(const String::value_type ch, String::size_type count)
{
    ASSERT(count > 0 && count < std::numeric_limits<size_type>::max());
    auto&& my_val = GetVal();
    if (count > my_val.capacity_)
    {
        Reserve(count);
    }
    auto ptr = my_val.GetPtr();
    char_traits::assign(ptr, count, ch);
    Eos(count);
}

void String::Construct(const String& right, size_type offset, size_type size)
{
    ASSERT(right.IsValidIndex(offset));
    size_type actual_size = math::Clamp<size_type>(size, 0, right.Length() - offset);
    if (actual_size > 0)
    {
        auto right_ptr = right.Data() + offset;
        Reserve(actual_size);
        char_traits::copy(Data(), right_ptr, actual_size);
    }
    Eos(actual_size);
}

void String::MoveConstruct(String& right, String::size_type offset, String::size_type size)
{
    ASSERT(right.IsValidIndex(offset));
    size_type actual_size = math::Clamp<size_type>(size, 0, right.Length() - offset);
    if (actual_size > 0)
    {
        auto right_ptr = right.Data();
        if (offset > 0)
        {
            char_traits::move(right_ptr, right_ptr + offset, actual_size);
            right.Eos(actual_size);
        }

        Reserve(actual_size);
        char_traits::move(Data(), right_ptr, actual_size);
    }
    Eos(actual_size);
}

void String::Assign(const char8_t* right, size_type length)
{
    ASSERT(IsValidAddress(right, right + length));
    Reserve(length);
    char_traits::copy(Data(), right, length);
    Eos(length);
}

void String::MoveAssign(String& right)
{
    ASSERT(IsValidAddress(right.Data(), right.Data() + right.Length()));
    size_type length = right.Length();
    Reserve(length);
    char_traits::move(Data(), right.Data(), length);
    Eos(length);
}

bool String::IsValidAddress(const char8_t* start, const char8_t* end) const
{
    auto my_start = Data();
    auto my_end = my_start + Length();
    return (start < my_start || start > my_end) && (end < my_start || end > my_end);
}

void String::Reallocate(String::size_type new_capacity)
{
    auto&& my_val = GetVal();
    auto&& alloc = GetAlloc();
    if (new_capacity > my_val.capacity_)
    {
        new_capacity = CalculateGrowth(new_capacity);
        if (!my_val.LargeStringEngaged())
        {
            pointer new_ptr = allocator_traits::allocate(alloc, new_capacity + 1);
            char_traits::move(new_ptr, my_val.u_.buffer_, my_val.size_ + 1);
            my_val.u_.ptr_ = new_ptr;
            my_val.capacity_ = new_capacity;
        }
        else
        {
            my_val.u_.ptr_ = allocator_traits::reallocate(alloc, my_val.u_.ptr_, my_val.capacity_ + 1, new_capacity + 1);
            my_val.capacity_ = new_capacity;
        }
    }
}

void String::ReallocateGrowth(size_type& increase_size)
{
    auto&& my_val = GetVal();
    size_type old_size = my_val.size_;
    if (MaxSize() - old_size < increase_size)
    {
        ASSERT(0);
        increase_size = MaxSize() - old_size;
    }

    size_type new_size = old_size + increase_size;
    size_type new_capacity = CalculateGrowth(new_size);

    auto&& alloc = GetAlloc();
    if (new_capacity > my_val.capacity_)
    {
        if (!my_val.LargeStringEngaged())
        {
            pointer new_ptr = allocator_traits::allocate(alloc, new_capacity + 1);
            char_traits::move(new_ptr, my_val.u_.buffer_, my_val.size_ + 1);
            my_val.u_.ptr_ = new_ptr;
            my_val.capacity_ = new_capacity;
        }
        else
        {
            my_val.u_.ptr_ = allocator_traits::reallocate(alloc, my_val.u_.ptr_, my_val.capacity_ + 1, new_capacity + 1);
            my_val.capacity_ = new_capacity;
        }
    }
    my_val.size_ = new_size;
    char_traits::assign(my_val.GetPtr()[new_size], value_type());
}
}
