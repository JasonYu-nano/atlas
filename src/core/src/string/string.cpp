// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "string/string.hpp"
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
    auto u8 = static_cast<value_type>(ch);
    Construct(&u8, 1);
}

String::String(const char8_t ch, String::size_type count)
{
    Construct(ch, count);
}

String::String(const char ch, String::size_type count)
{
    Construct(static_cast<value_type>(ch), count);
}

String::String(const char8_t* str)
{
    Construct(str, char_traits::length(str));
}

String::String(const char8_t* str, String::size_type length)
{
    Construct(str, length);
}

String::String(const char* str)
{
    Construct(reinterpret_cast<const char8_t*>(str), std::char_traits<char>::length(str));
}

String::String(const char* str, String::size_type length)
{
    Construct(reinterpret_cast<const char8_t*>(str), length);
}

String::String(const String& right)
{
    Construct(right, 0, std::numeric_limits<size_type>::max());
}

String::String(const String& right, size_type offset, size_type size)
{
    Construct(right, offset, size);
}

String::String(String&& right) noexcept
{
    MoveConstruct(right, 0, std::numeric_limits<size_type>::max());
}

String::String(String&& right, size_type offset, size_type size) noexcept
{
    MoveConstruct(right, offset, size);
}

String::~String()
{
    auto&& my_val = GetVal();
    if (LargeStringEngaged())
    {
        allocator_traits::deallocate(GetAlloc(), my_val.GetPtr(), my_val.capacity_ + 1);
    }
    my_val.size_ = 0;
    my_val.capacity_ = val_type::INLINE_SIZE - 1;
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
    Assign(reinterpret_cast<const char8_t*>(right), static_cast<size_type>(std::char_traits<char>::length(right)));
    return *this;
}

String& String::operator=(const char8_t* right)
{
    Assign(right, char_traits::length(right));
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

bool String::Equals(const String &right, ECaseSensitive case_sensitive) const
{
    const size_type left_length = Length();
    const size_type right_length = right.Length();

    if (left_length != right_length)
    {
        return false;
    }

    if (case_sensitive == ECaseSensitive::Sensitive)
    {
        return char_traits::compare(Data(), right.Data(), left_length) == 0;
    }

    String left_fold_case = FoldCase();
    String right_fold_case = right.FoldCase();
    return char_traits::compare(left_fold_case.Data(), right_fold_case.Data(), left_length) == 0;
}

void String::Reserve(String::size_type capacity)
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
            pointer new_ptr = allocator_traits::allocate(GetAlloc(), capacity + 1);
            pointer old_ptr = my_val.u_.ptr_;
            char_traits::move(new_ptr, old_ptr, my_val.size_ + 1);
            allocator_traits::deallocate(GetAlloc(), old_ptr, my_val.capacity_ + 1);
            my_val.u_.ptr_ = new_ptr;
            my_val.capacity_ = capacity;
        }
    }
}

String String::FoldCase() const
{
    const char* data = reinterpret_cast<const char*>(Data());
    std::string fold_case = boost::locale::fold_case(data, data + Length(), locale::DefaultLocale());
    return {fold_case.data(), static_cast<size_type>(fold_case.length())};
}

String String::ToUpper(const std::locale& locale) const
{
    const char* data = reinterpret_cast<const char*>(Data());
    std::string upper = boost::locale::to_upper(data, data + Length(), locale);
    return {upper.data(), static_cast<size_type>(upper.length())};
}
String String::ToLower(const std::locale& locale) const
{
    const char* data = reinterpret_cast<const char*>(Data());
    std::string lower = boost::locale::to_lower(data, data + Length(), locale);
    return {lower.data(), static_cast<size_type>(lower.length())};
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
    return {u8.data(), static_cast<size_type>(u8.length())};
}

String String::FromUtf32(const char32_t* str, size_type length)
{
    if (length <= 0)
    {
        return {};
    }

    std::string u8 = boost::locale::conv::utf_to_utf<char, char32_t>(str, str + length);
    return {u8.data(), static_cast<size_type>(u8.length())};
}

void String::BecomeLarge(String::size_type capacity)
{
    auto&& my_val = GetVal();
    pointer new_ptr = allocator_traits::allocate(GetAlloc(), capacity + 1);
    char_traits::move(new_ptr, my_val.u_.buffer_, my_val.size_ + 1);
    my_val.u_.ptr_ = new_ptr;
    my_val.capacity_ = capacity;
}

void String::Construct(String::const_pointer str, allocator_traits::size_type len)
{
    ASSERT(len < std::numeric_limits<size_type>::max());
    Reserve(len + 1);
    auto&& my_val = GetVal();
    pointer ptr = my_val.GetPtr();
    char_traits::copy(ptr, str, len);
    Eos(len);
}

void String::Construct(const String::value_type ch, String::size_type count)
{
    ASSERT(count > 0 && count < std::numeric_limits<size_type>::max());
    Reserve(count + 1);
    auto&& my_val = GetVal();
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
        Reserve(actual_size + 1);
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

        Reserve(actual_size + 1);
        char_traits::move(Data(), right_ptr, actual_size);
    }
    Eos(actual_size);
}

void String::Assign(const char8_t* right, size_type length)
{
    ASSERT(IsValidAddress(right, right + length));
    Reserve(length + 1);
    char_traits::copy(Data(), right, length);
    Eos(length);
}

void String::MoveAssign(String& right)
{
    ASSERT(IsValidAddress(right.Data(), right.Data() + right.Length()));
    size_type length = right.Length();
    Reserve(length + 1);
    char_traits::move(Data(), right.Data(), length);
    Eos(length);
}

void String::Eos(String::size_type size)
{
    auto&& my_val = GetVal();
    my_val.size_ = size;
    char_traits::assign(my_val.GetPtr()[size], value_type());
}

bool String::IsValidAddress(const char8_t* start, const char8_t* end) const
{
    auto my_start = Data();
    auto my_end = my_start + Length();
    return (start < my_start || start > my_end) && (end < my_start || end > my_end);
}

}
