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

String::String(const char8_t* str)
{
    Construct(str, CharTraits::length(str));
}

String::String(const char8_t* str, String::SizeType length)
{
    Construct(str, length);
}

String::String(const char* str)
{
    Construct(reinterpret_cast<const char8_t*>(str), std::char_traits<char>::length(str));
}

String::String(const char* str, String::SizeType length)
{
    Construct(reinterpret_cast<const char8_t*>(str), length);
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
    Assign(right, CharTraits::length(right));
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
    auto it = reinterpret_cast<const char*>(Data());
    auto end = it + Length();
    SizeType count = 0;

    while (it < end)
    {
        int32 next = UtfTraits<char>::trail_length(*it) + 1;
        it += next;
        ++count;
    }

    return count;
}

CodePoint String::CodePointAt(std::make_unsigned_t<SizeType> offset) const
{
    auto it = reinterpret_cast<const char*>(Data());
    auto end = it + Length();
    SizeType current_offset = 0;

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
    const SizeType left_length = Length();
    const SizeType right_length = right.Length();

    if (left_length != right_length)
    {
        return false;
    }

    if (case_sensitive == ECaseSensitive::Sensitive)
    {
        return CharTraits::compare(Data(), right.Data(), left_length) == 0;
    }

    String left_fold_case = FoldCase();
    String right_fold_case = right.FoldCase();
    return CharTraits::compare(left_fold_case.Data(), right_fold_case.Data(), left_length) == 0;
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

String String::FoldCase() const
{
    const char* data = reinterpret_cast<const char*>(Data());
    std::string fold_case = boost::locale::fold_case(data, data + Length(), locale::DefaultLocale());
    return {fold_case.data(), static_cast<SizeType>(fold_case.length())};
}

String String::ToUpper(const std::locale& locale) const
{
    const char* data = reinterpret_cast<const char*>(Data());
    std::string upper = boost::locale::to_upper(data, data + Length(), locale);
    return {upper.data(), static_cast<SizeType>(upper.length())};
}
String String::ToLower(const std::locale& locale) const
{
    const char* data = reinterpret_cast<const char*>(Data());
    std::string lower = boost::locale::to_lower(data, data + Length(), locale);
    return {lower.data(), static_cast<SizeType>(lower.length())};
}

String& String::Remove(SizeType from, SizeType count)
{
    ASSERT(count > 0 && from < Length() && from + count <= Length());

    Pointer start = Data() + from;
    SizeType length = Length();
    SizeType new_length = length - count;
    CharTraits::move(start, start + count, length - from - count);
    Eos(new_length);

    return *this;
}

String String::FromUtf16(const char16_t* str, SizeType length)
{
    if (length <= 0)
    {
        return {};
    }

    std::string u8 = boost::locale::conv::utf_to_utf<char, char16_t>(str, str + length);
    return {u8.data(), static_cast<SizeType>(u8.length())};
}

String String::FromUtf32(const char32_t* str, SizeType length)
{
    if (length <= 0)
    {
        return {};
    }

    std::string u8 = boost::locale::conv::utf_to_utf<char, char32_t>(str, str + length);
    return {u8.data(), static_cast<SizeType>(u8.length())};
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
    ASSERT(len < std::numeric_limits<SizeType>::max());
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
    SizeType actual_size = math::Clamp<SizeType>(size, 0, right.Length() - offset);
    if (actual_size > 0)
    {
        auto right_ptr = right.Data() + offset;
        Reserve(actual_size + 1);
        CharTraits::copy(Data(), right_ptr, actual_size);
    }
    Eos(actual_size);
}

void String::MoveConstruct(String& right, String::SizeType offset, String::SizeType size)
{
    ASSERT(right.IsValidIndex(offset));
    SizeType actual_size = math::Clamp<SizeType>(size, 0, right.Length() - offset);
    if (actual_size > 0)
    {
        auto right_ptr = right.Data();
        if (offset > 0)
        {
            CharTraits::move(right_ptr, right_ptr + offset, actual_size);
            right.Eos(actual_size);
        }

        Reserve(actual_size + 1);
        CharTraits::move(Data(), right_ptr, actual_size);
    }
    Eos(actual_size);
}

void String::Assign(const char8_t* right, SizeType length)
{
    ASSERT(IsValidAddress(right, right + length));
    Reserve(length + 1);
    CharTraits::copy(Data(), right, length);
    Eos(length);
}

void String::MoveAssign(String& right)
{
    ASSERT(IsValidAddress(right.Data(), right.Data() + right.Length()));
    SizeType length = right.Length();
    Reserve(length + 1);
    CharTraits::move(Data(), right.Data(), length);
    Eos(length);
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
