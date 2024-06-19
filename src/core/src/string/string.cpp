// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "string/string.hpp"
#include "math/atlas_math.hpp"

namespace atlas
{

namespace
{
    int32 icompare(String::const_pointer lhs, String::const_pointer rhs, String::size_type length)
    {
        auto&& fold_case = details::FoldCaseUnsafe<String::value_type>();
        auto&& loc = locale::default_locale();
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

String::size_type String::count() const
{
    auto it = data();
    auto end = it + length();
    size_type count = 0;

    while (it < end)
    {
        int32 next = UtfTraits<char>::trail_length(*it) + 1;
        it += next;
        ++count;
    }

    return count;
}

CodePoint String::code_point_at(std::make_unsigned_t<size_type> offset) const
{
    auto it = data();
    auto end = it + length();
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

int32 String::compare(const String& right, ECaseSensitive case_sensitive) const
{
    const size_type left_length = length();
    const size_type right_length = right.length();

    if (left_length != right_length)
    {
        return left_length < right_length ? -1 : 1;
    }

    if (case_sensitive == ECaseSensitive::Sensitive)
    {
        return char_traits::compare(data(), right.data(), left_length);
    }

    return icompare(data(), right.data(), left_length);
}

String String::fold_case() const
{
    std::string fold_case = boost::locale::fold_case(data(), data() + length(), locale::default_locale());
    return {fold_case.data(), convert_size(fold_case.length())};
}

String String::to_upper(const std::locale& locale) const
{
    std::string upper = boost::locale::to_upper(data(), data() + length(), locale);
    return {upper.data(), convert_size(upper.length())};
}

String String::to_lower(const std::locale& locale) const
{
    std::string lower = boost::locale::to_lower(data(), data() + length(), locale);
    return {lower.data(), convert_size(lower.length())};
}

std::wstring String::to_wide() const
{
    auto&& my_val = get_val();
    size_type length = my_val.size_;
    if (length <= 0)
    {
        return {};
    }

    return boost::locale::conv::utf_to_utf<wchar_t, char>(data(), data() + length);
}

std::u16string String::to_utf16() const
{
    auto&& my_val = get_val();
    size_type length = my_val.size_;
    if (length <= 0)
    {
        return {};
    }

    return boost::locale::conv::utf_to_utf<char16_t, char>(data(), data() + length);
}

std::u32string String::to_utf32() const
{
    auto&& my_val = get_val();
    size_type length = my_val.size_;
    if (length <= 0)
    {
        return {};
    }

    return boost::locale::conv::utf_to_utf<char32_t, char>(data(), data() + length);
}

String& String::remove(size_type from, size_type count)
{
    ASSERT(count > 0 && from < length() && from + count <= length());

    pointer start = data() + from;
    size_type len = length();
    size_type new_length = len - count;
    char_traits::move(start, start + count, len - from - count);
    eos(new_length);

    return *this;
}

void String::construct(const_pointer str, size_type len)
{
    ASSERT(len < std::numeric_limits<size_type>::max());
    auto&& my_val = get_val();
    if (len > my_val.capacity_)
    {
        reserve(len);
    }
    pointer ptr = my_val.get_ptr();
    char_traits::move(ptr, str, len);
    eos(len);
}

void String::construct(value_type ch, size_type count)
{
    ASSERT(count > 0 && count < std::numeric_limits<size_type>::max());
    auto&& my_val = get_val();
    if (count > my_val.capacity_)
    {
        reserve(count);
    }
    auto ptr = my_val.get_ptr();
    char_traits::assign(ptr, count, ch);
    eos(count);
}

void String::construct(const String& right, size_type offset, size_type size)
{
    ASSERT(right.is_valid_index(offset));
    size_type actual_size = math::clamp<size_type>(size, 0, right.length() - offset);
    if (actual_size > 0)
    {
        auto right_ptr = right.data() + offset;
        reserve(actual_size);
        char_traits::copy(data(), right_ptr, actual_size);
    }
    eos(actual_size);
}

void String::move_construct(String& right, String::size_type offset, String::size_type size)
{
    ASSERT(right.is_valid_index(offset));
    size_type actual_size = math::clamp<size_type>(size, 0, right.length() - offset);
    if (actual_size > 0)
    {
        auto right_ptr = right.data();
        if (offset > 0)
        {
            char_traits::move(right_ptr, right_ptr + offset, actual_size);
            right.eos(actual_size);
        }

        reserve(actual_size);
        char_traits::move(data(), right_ptr, actual_size);
    }
    eos(actual_size);
}

void String::assign(const_pointer right, size_type length)
{
    ASSERT(is_valid_address(right, right + length));
    reserve(length);
    char_traits::copy(data(), right, length);
    eos(length);
}

void String::move_assign(String& right)
{
    ASSERT(is_valid_address(right.data(), right.data() + right.length()));
    size_type length = right.length();
    reserve(length);
    char_traits::move(data(), right.data(), length);
    eos(length);
}

bool String::is_valid_address(const_pointer start, const_pointer end) const
{
    auto my_start = data();
    auto my_end = my_start + length();
    return (start < my_start || start > my_end) && (end < my_start || end > my_end);
}

String::size_type String::calculate_growth(size_type requested) const
{
    if (requested > max_size())
    {
        return max_size();
    }

    size_type old = get_val().capacity_;
    if (old > max_size() - old / 2)
    {
        return max_size();
    }

    return math::max(requested, old + old / 2);
}

void String::reallocate(String::size_type new_capacity)
{
    auto&& my_val = get_val();
    auto&& alloc = get_alloc();
    if (new_capacity > my_val.capacity_)
    {
        new_capacity = calculate_growth(new_capacity);
        if (!my_val.large_string_engaged())
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

void String::reallocate_growth(size_type& increase_size)
{
    auto&& my_val = get_val();
    size_type old_size = my_val.size_;
    if (max_size() - old_size < increase_size)
    {
        ASSERT(0);
        increase_size = max_size() - old_size;
    }

    size_type new_size = old_size + increase_size;
    size_type new_capacity = calculate_growth(new_size);

    if (new_capacity > my_val.capacity_)
    {
        auto&& alloc = get_alloc();
        if (!my_val.large_string_engaged())
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
    char_traits::assign(my_val.get_ptr()[new_size], value_type());
}
}
