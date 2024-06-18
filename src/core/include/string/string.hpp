// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include <string_view>
#include <ranges>

#include "fmt/format.h"
#include "boost/algorithm/string/find.hpp"
#include "boost/algorithm/string/finder.hpp"

#include "string/string_utility.hpp"
#include "utility/compression_pair.hpp"
#include "memory/allocator.hpp"
#include "string/locale.hpp"
#include "string/unicode.hpp"
#include "misc/iterator.hpp"
#include "math/atlas_math.hpp"
#include "core_macro.hpp"

namespace atlas
{
namespace details
{

template<typename T, bool = sizeof(T) == sizeof(char), bool = sizeof(T) == sizeof(wchar_t)>
struct FoldCaseUnsafeSwitcher;

template<typename T>
struct FoldCaseUnsafeSwitcher<T, true, false>
{
    static constexpr int32 value = 1;
};

template<typename T>
struct FoldCaseUnsafeSwitcher<T, false, true>
{
    static constexpr int32 value = 2;
};

template<typename CharType, int32>
struct FoldCaseUnsafeImpl
{
    CharType operator() (const CharType ch, const std::locale& loc)
    {
        return std::tolower(ch, loc);
    }
};

template<typename CharType>
struct FoldCaseUnsafeImpl<CharType, 1>
{
    CharType operator() (const CharType ch, const std::locale& loc)
    {
        return static_cast<CharType>(std::tolower(static_cast<char>(ch), loc));
    }
};

template<typename CharType>
struct FoldCaseUnsafeImpl<CharType, 2>
{
    CharType operator() (const CharType ch, const std::locale& loc)
    {
        return static_cast<CharType>(std::tolower(static_cast<wchar_t>(ch), loc));
    }
};

template<typename CharType>
struct FoldCaseUnsafe : private FoldCaseUnsafeImpl<CharType, FoldCaseUnsafeSwitcher<CharType>::value>
{
private:
    using base = FoldCaseUnsafeImpl<CharType, FoldCaseUnsafeSwitcher<CharType>::value>;
public:
    using base::operator();
};

class EqualsInsensitive
{
public:
    explicit EqualsInsensitive(const std::locale& loc) : loc_(loc) {}

    template<typename T1, typename T2>
    bool operator()(const T1& ch1, const T2& ch2) const
    {
        return FoldCaseUnsafe<T1>()(ch1, loc_) == FoldCaseUnsafe<T2>()(ch2, loc_);
    }

private:
    const std::locale& loc_;
};

template<typename CharTraits, typename SizeType>
class StringVal
{
    using char_type = typename CharTraits::char_type;
public:
    static constexpr size_t inline_size_ = (16 / sizeof(char_type) < 1) ? 1 : 16 / sizeof(char_type);

    bool large_string_engaged() const { return inline_size_ <= capacity_; }
    char_type* get_ptr() { return large_string_engaged() ? u_.ptr_ : u_.buffer_; }
    const char_type* get_ptr() const { return large_string_engaged() ? u_.ptr_ : u_.buffer_; }

    SizeType size_{ 0 };
    SizeType capacity_{ inline_size_ - 1 };

    union UnionBuffer
    {
        char_type buffer_[inline_size_];
        char_type* ptr_;
    } u_;
};

}

enum class ECaseSensitive : uint8
{
    Sensitive,
    Insensitive
};

/**
 * @brief Represents text as a sequence of zero terminated UTF-8 code units.
 */
class CORE_API String
{
public:
    using value_type                = char;
    using char_traits               = std::char_traits<value_type>;
    using allocator_type            = HeapAllocator<value_type>;
    using allocator_traits          = AllocatorTraits<allocator_type>;
    using size_type                 = allocator_traits::size_type;
    using view_type                 = BasicStringView<value_type>;
    using pointer                   = value_type*;
    using const_pointer             = const value_type*;
    using iterator                  = PointerIterator<value_type>;
    using const_iterator            = ConstPointerIterator<value_type>;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

private:
    using val_type                  = details::StringVal<char_traits, size_type>;
    using param_type                = CallTraits<value_type>::param_type;

public:
    String() noexcept { eos(0); }
    explicit String(value_type ch)
    {
        construct(ch, 1);
    }

    String(value_type ch, size_type count)
    {
        construct(ch, count);
    }

    String(const_pointer str)
    {
        construct(str, convert_size(char_traits::length(str)));
    }
    String(const_pointer str, size_type length)
    {
        construct(str, length);
    }
    explicit String(BasicStringView<value_type> view)
    {
        construct(view.data(), view.length());
    }

    String(const String& right)
    {
        construct(right, 0, max_size());
    }
    String(const String& right, size_type offset, size_type size = max_size())
    {
        construct(right, offset, size);
    }
    String(String&& right) noexcept : pair_(OneThenVariadicArgs(), std::move(right.get_alloc())
        , std::exchange(right.get_val().size_, 0)
        , std::exchange(right.get_val().capacity_, 0)
        , std::exchange(right.get_val().u_, val_type::UnionBuffer()))
    {}
    String(String&& right, size_type offset, size_type size = max_size()) noexcept
    {
        move_construct(right, offset, size);
    }

    ~String() noexcept
    {
        auto&& my_val = get_val();
        if (my_val.large_string_engaged())
        {
            allocator_traits::deallocate(get_alloc(), my_val.get_ptr(), my_val.capacity_ + 1);
        }
        my_val.size_ = 0;
        my_val.capacity_ = val_type::inline_size_ - 1;
    }

    String& operator= (const String& right)
    {
        assign(right.Data(), right.length());
        return *this;
    }
    String& operator= (String&& right) noexcept
    {
        move_assign(right);
        return *this;
    }
    String& operator= (const_pointer right)
    {
        assign(right, char_traits::length(right));
        return *this;
    }
    String& operator= (view_type right)
    {
        assign(right.data(), right.length());
        return *this;
    }

    operator view_type() const
    {
        return {Data(), length()};
    }

    bool operator== (const String& right) const { return equals(right, ECaseSensitive::Sensitive); }
    bool operator!= (const String& right) const { return !equals(right, ECaseSensitive::Sensitive); }
    bool operator< (const String& right) const  { return compare(right, ECaseSensitive::Sensitive) < 0; }
    bool operator> (const String& right) const  { return compare(right, ECaseSensitive::Sensitive) > 0; }

    value_type& operator[] (size_type index)
    {
        ASSERT(is_valid_index(index));
        return get_val().get_ptr()[index];
    }
    value_type operator[] (size_type index) const
    {
        ASSERT(is_valid_index(index));
        return get_val().get_ptr()[index];
    }

    String operator+ (value_type ch) const
    {
        return concat(view_type(&ch, 1));
    }

    String operator+ (const_pointer ptr) const
    {
        return concat(ptr);
    }

    template<std::ranges::contiguous_range Range>
    String operator+ (const Range& range) const
    {
        return concat(range);
    }

    String& operator+= (value_type ch)
    {
        return append(view_type(&ch, 1));
    }

    String& operator+= (const_pointer ptr)
    {
        return append(ptr);
    }

    template<std::ranges::contiguous_range Range>
    String& operator+= (const Range& range)
    {
        return append(range);
    }

    NODISCARD inline pointer Data()                    { return get_val().get_ptr(); }
    NODISCARD inline const_pointer Data() const         { return get_val().get_ptr(); }
    NODISCARD DO_NOT_USE_DIRECTLY inline pointer data()                    { return get_val().get_ptr(); }
    NODISCARD DO_NOT_USE_DIRECTLY inline const_pointer data() const         { return get_val().get_ptr(); }

    NODISCARD iterator begin()                          { return iterator(Data()); }
    NODISCARD const_iterator begin() const              { return const_iterator(Data()); }
    NODISCARD iterator end()                            { return iterator(Data() + length()); }
    NODISCARD const_iterator end() const                { return const_iterator(Data() + length()); }

    NODISCARD reverse_iterator rbegin()                 { return reverse_iterator(end()); }
    NODISCARD const_reverse_iterator rbegin() const     { return const_reverse_iterator(end()); }
    NODISCARD reverse_iterator rend()                   { return reverse_iterator(begin()); }
    NODISCARD const_reverse_iterator rend() const       { return const_reverse_iterator(begin()); }

    NODISCARD const_iterator cbegin() const             { return begin(); }
    NODISCARD const_iterator cend() const               { return end(); }
    NODISCARD const_reverse_iterator crbegin() const    { return const_reverse_iterator(end()); }
    NODISCARD const_reverse_iterator crend() const      { return const_reverse_iterator(begin()); }

    /**
     * @brief Gets the number of characters in the current string.
     * @return
     */
    size_type length() const { return get_val().size_; }
    /**
     * @brief Gets the number of characters in the current string.
     * @return
     */
    size_type size() const { return get_val().size_; }

    constexpr static size_t max_size() { return std::numeric_limits<size_t>::max() - 1; }
    /**
     * @brief Check if the string is empty.
     * @return
     */
    NODISCARD bool is_empty() const
    {
        auto&& my_val = get_val();
        return my_val.size_ <= 0;
    }
    /**
     * @brief Get the number of code points contained in the string.
     * @return
     */
    NODISCARD size_type count() const;

    /**
     * @brief Get the capacity of the current string.
     * @return
     */
    size_type capacity() const { return get_val().capacity_; }

    /**
     * @brief Get code point by offset.
     * @param offset
     * @return
     */
    NODISCARD CodePoint code_point_at(std::make_unsigned_t<size_type> offset) const;

    /**
     * @brief Determines whether this instance and another specified string have the same value.
     * @param right
     * @param case_sensitive
     * @return
     */
    NODISCARD bool equals(const String& right, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return compare(right, case_sensitive) == 0;
    }

    /**
     * @brief Compare with another string.
     * @param right
     * @param case_sensitive
     * @return Positive number if larger than another string.
     */
    NODISCARD int32 compare(const String& right, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    /**
     * @brief If new capacity is greater than the current capacity, new storage is allocated, and capacity is made equal or greater than new capacity.
     * @param capacity
     */
    void reserve(size_type capacity)
    {
        if (capacity > get_val().capacity_)
        {
            reallocate(capacity);
        }
    }

    NODISCARD bool is_valid_index(size_type index) const { return index < length(); }

    /**
     * @brief Fold the current string.
     * @return A new string.
     */
    NODISCARD String fold_case() const;

    /**
     * @brief Check if the string is uppercase in specified locale.
     * @param locale
     * @return
     */
    NODISCARD bool is_upper(const std::locale& locale = locale::default_locale()) const;
    /**
     * @brief Translate the current string to uppercase.
     * @param locale
     * @return A new uppercase string.
     */
    NODISCARD String to_upper(const std::locale& locale = locale::default_locale()) const;
    /**
     * @brief Check if the string is lowercase in specified locale.
     * @param locale
     * @return
     */
    NODISCARD bool is_lower(const std::locale& locale = locale::default_locale()) const;
    /**
     * @brief Translate the current string to lowercase.
     * @param locale
     * @return A new lowercase string.
     */
    NODISCARD String to_lower(const std::locale& locale = locale::default_locale()) const;
    NODISCARD std::string ToStdString() const
    {
        return { Data(), length() };
    }
    NODISCARD std::wstring to_wide() const;
    NODISCARD std::u16string to_utf16() const;
    NODISCARD std::u32string to_utf32() const;

    /**
     * @brief Append the specified characters to the current string.
     * @param ch
     * @return
     */
    String& append(value_type ch)  { return append(view_type(&ch, 1)); }
    /**
     * @brief Append the specified string to the current string.
     * @param str
     * @return Current string
     */
    String& append(const_pointer str)  { return append(view_type(str)); }
    /**
     * @brief Append the specified range to the current string.
     * @tparam RangeType
     * @param range
     * @return Current string
     */
    template<std::ranges::contiguous_range RangeType>
    String& append(const RangeType& range)
    {
        size_type len = length();
        size_type increase_size = convert_size(range.size());
        auto&& my_val = get_val();
        if (increase_size > my_val.capacity_ - my_val.size_)
        {
            reallocate_growth(increase_size);
        }
        else
        {
            eos(len + increase_size);
        }

        char_traits::copy(my_val.get_ptr() + len, reinterpret_cast<const_pointer>(range.data()), increase_size);
        return *this;
    }
    /**
     * @brief Prepend the specified characters to the current string.
     * @param ch
     * @return
     */
    String& prepend(value_type ch) { return prepend(view_type(&ch, 1)); }
    /**
     * @brief Prepend the specified string to the current string.
     * @param str
     * @return Current string
     */
    String& prepend(const_pointer str) { return prepend(view_type(str)); }
    /**
     * @brief Prepend the specified range to the current string.
     * @tparam RangeType
     * @param range
     * @return Current string
     */
    template<std::ranges::contiguous_range RangeType>
    String& prepend(const RangeType& range)
    {
        size_type len = length();
        size_type increase_size = convert_size(range.size());
        auto&& my_val = get_val();
        if (increase_size > my_val.capacity_ - my_val.size_)
        {
            reallocate_growth(increase_size);
        }
        else
        {
            eos(len + increase_size);
        }

        pointer ptr = my_val.get_ptr();
        char_traits::move(ptr + increase_size, ptr, len);
        char_traits::copy(ptr, reinterpret_cast<const_pointer>(range.data()), increase_size);
        return *this;
    }
    /**
     * @brief Concatenates specified const string and characters.
     * @param ch
     * @return
     */
    NODISCARD String concat(value_type ch) const { return concat(view_type(&ch, 1)); }
    /**
     * @brief Concatenates two specified const string.
     * @param str
     * @return A new string
     */
    NODISCARD String concat(const_pointer str) const { return concat(view_type(str)); }
    /**
     * @brief Concatenate the current string with the specified range.
     * @tparam RangeType
     * @param range
     * @return A new string
     */
    template<std::ranges::contiguous_range RangeType>
    NODISCARD String concat(const RangeType& range) const
    {
        size_type len = length();
        size_type increase_size = convert_size(range.size());
        if (increase_size > max_size() - len)
        {
            ASSERT(0);
            increase_size = max_size() - len;
        }

        size_type new_length = len + increase_size;

        String result;
        result.reserve(new_length);

        pointer ptr = result.Data();
        char_traits::copy(ptr, Data(), len);
        char_traits::copy(ptr + len, reinterpret_cast<const_pointer>(range.data()), increase_size);
        result.eos(new_length);

        return result;
    }
    /**
     * @brief Insert the specified string into the current string.
     * @param offset
     * @param str
     * @return Current string
     */
    String& insert(size_type offset, const_pointer str) { return insert(cbegin() + offset, view_type(str)); }
    /**
     * @brief Insert the specified range into the current string.
     * @tparam RangeType
     * @param where
     * @param range
     * @return Current string
     */
    template<std::ranges::contiguous_range RangeType>
    String& insert(const const_iterator& where, const RangeType& range)
    {
        ASSERT(where >= cbegin() && where <= cend());
        auto&& my_val = get_val();
        size_type increase_size = convert_size(range.size());
        size_type offset = convert_size(std::distance(cbegin(), where));
        size_type length = my_val.size_;

        if (increase_size > my_val.capacity_ - my_val.size_)
        {
            reallocate_growth(increase_size);
        }

        pointer ptr = my_val.get_ptr() + offset;
        char_traits::move(ptr + increase_size, ptr, length - offset);
        char_traits::copy(ptr, reinterpret_cast<const_pointer>(range.data()), increase_size);

        return *this;
    }

    /**
     * @brief Remove a specified number of characters from the specified position.
     * @param from
     * @param count
     * @return Current string
     */
    String& remove(size_type from, size_type count);
    /**
     * @brief Remove all specified strings from the current string.
     * @param str
     * @param case_sensitive
     * @return
     */
    String& remove(const_pointer str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive)
    {
        return remove(view_type(str), case_sensitive);
    }
    /**
     * @brief Remove all specified ranges from the current string.
     * @tparam RangeType
     * @param range
     * @param case_sensitive
     * @return
     */
    template<std::ranges::sized_range RangeType>
    String& remove(const RangeType& range, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive)
    {
        size_type index = 0;
        size_type size = convert_size(range.size());
        do
        {
            index = find(range, index, case_sensitive);
            if (index == INDEX_NONE)
            {
                break;
            }

            remove(index, size);

        } while (true);
        return *this;
    }
    /**
     * @brief Replace specified number of characters in the current instance with another specified string.
     * @param from
     * @param count
     * @param new_value
     * @return
     */
    String& replace(size_type from, size_type count, const_pointer new_value)
    {
        return replace(from, count, view_type(new_value));
    }
    /**
     * @brief Replace specified number of characters in the current instance with another specified string.
     * @tparam RangeType
     * @param from
     * @param count
     * @param new_value
     * @return
     */
    template<std::ranges::contiguous_range RangeType>
    String& replace(size_type from, size_type count, const RangeType& new_value)
    {
        ASSERT(count > 0 && from < length() && from <= length() - count);

        size_type len = length();
        size_type insert_size = convert_size(new_value.size());
        if (insert_size > count)
        {
            size_type increase_size = insert_size - count;
            if (increase_size > max_size() - len)
            {
                ASSERT(0);
                insert_size = max_size() - len + count;
            }
        }
        size_type new_length = len - count + insert_size;
        reserve(new_length);

        pointer start = Data() + from;
        char_traits::move(start + insert_size, start + count, len - from - count);
        char_traits::copy(start, reinterpret_cast<const_pointer>(new_value.data()), insert_size);

        eos(new_length);

        return *this;
    }
    /**
     * @brief Replace all occurrences of a specified string in the current instance with another specified string.
     * @param old_value
     * @param new_value
     * @return
     */
    String& replace(value_type old_value, value_type new_value)
    {
        return replace(view_type(&old_value, 1), view_type(&new_value, 1));
    }
    /**
     * @brief Replace all occurrences of a specified string in the current instance with another specified string.
     * @param old_value
     * @param new_value
     * @return
     */
    String& replace(const_pointer old_value, const_pointer new_value)
    {
        return replace(view_type(old_value), view_type(new_value));
    }
    /**
     * @brief Replace all occurrences of a specified string in the current instance with another specified string.
     * @tparam RangeType1
     * @tparam RangeType2
     * @param old_value
     * @param new_value
     * @param case_sensitive
     * @return
     */
    template<std::ranges::sized_range RangeType1, std::ranges::contiguous_range RangeType2>
    String& replace(const RangeType1& old_value, const RangeType2& new_value, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive)
    {
        size_type index = 0;
        size_type size = old_value.size();
        do
        {
            index = find(old_value, index, case_sensitive);
            if (index == INDEX_NONE)
            {
                break;
            }

            replace(index, size, new_value);

        } while (true);
        return *this;
    }
    /**
     * @brief Determines whether the beginning of this string instance matches the specified string.
     * @param str
     * @param case_sensitive
     * @return
     */
    bool starts_with(const_pointer str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return starts_with(view_type(str), case_sensitive);
    }
    /**
     * @brief Determines whether the beginning of this string instance matches the specified range.
     * @tparam RangeType
     * @param range
     * @param case_sensitive
     * @return
     */
    template<std::ranges::sized_range RangeType>
    bool starts_with(const RangeType& range, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        if (length() < range.size())
        {
            return false;
        }

        const_iterator p = cbegin();
        const std::locale& loc = locale::default_locale();
        auto&& my_fold_case = details::FoldCaseUnsafe<value_type>();
        auto&& range_fold_case = details::FoldCaseUnsafe<typename RangeType::value_type>();
        for (auto&& it = range.begin(); it < range.end(); ++it, ++p)
        {
            if (case_sensitive == ECaseSensitive::Sensitive
                ? *it != *p
                : range_fold_case(*it, loc) != my_fold_case(*p, loc))
            {
                return false;
            }
        }
        return true;
    }
    /**
     * @brief Determines whether the end of this string instance matches the specified string.
     * @param str
     * @param case_sensitive
     * @return
     */
    bool ends_with(const_pointer str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return ends_with(view_type(str), case_sensitive);
    }
    /**
     * @brief Determines whether the end of this string instance matches the specified range.
     * @tparam RangeType
     * @param range
     * @param case_sensitive
     * @return
     */
    template<std::ranges::sized_range RangeType>
    bool ends_with(const RangeType& range, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        if (length() < range.size())
        {
            return false;
        }

        const_reverse_iterator p = crbegin();
        const std::locale& loc = locale::default_locale();
        auto&& my_fold_case = details::FoldCaseUnsafe<value_type>();
        auto&& range_fold_case = details::FoldCaseUnsafe<typename RangeType::value_type>();
        for (auto&& it = std::make_reverse_iterator(range.end()); it < std::make_reverse_iterator(range.begin()); ++it, ++p)
        {
            if (case_sensitive == ECaseSensitive::Sensitive
                ? *it != *p
                : range_fold_case(*it, loc) != my_fold_case(*p, loc))
            {
                return false;
            }
        }
        return true;
    }

    template<std::ranges::range RangeType>
    size_type find(const RangeType& search, size_type offset, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        auto&& source = boost::make_iterator_range(begin() + offset, end());
        auto&& range = case_sensitive == ECaseSensitive::Sensitive
                       ? boost::algorithm::find_first(source, search)
                       : boost::algorithm::find(source, ::boost::algorithm::first_finder(search, details::EqualsInsensitive(locale::default_locale())));

        return range.empty() ? size_type(INDEX_NONE) : std::distance(cbegin(), range.begin());
    }

    template<std::ranges::range RangeType>
    size_type find_last(const RangeType& search, size_type offset_to_tail, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        auto&& source = boost::make_iterator_range(begin(), end() - offset_to_tail);
        auto&& range = case_sensitive == ECaseSensitive::Sensitive
                       ? boost::algorithm::find_last(source, search)
                       : boost::algorithm::find(source, ::boost::algorithm::last_finder(search, details::EqualsInsensitive(locale::default_locale())));

        return range.empty() ? size_type(INDEX_NONE) : std::distance(cbegin(), range.begin());
    }
    /**
     * @brief Reports the zero-based index of the first occurrence of the specified string in this instance.
     * @param search
     * @param case_sensitive
     * @return
     */
    size_type index_of(const_pointer search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return index_of(view_type(search), case_sensitive);
    }
    /**
     * @brief Reports the zero-based index of the first occurrence of the specified string in this instance.
     * @tparam RangeType
     * @param search
     * @param case_sensitive
     * @return
     */
    template<std::ranges::range RangeType>
    size_type index_of(const RangeType& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return find(search, 0, case_sensitive);
    }
    /**
     * @brief Reports the zero-based index position of the last occurrence of a specified string within this instance.
     * @param search
     * @param case_sensitive
     * @return
     */
    size_type last_index_of(const_pointer search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return last_index_of(view_type(search), case_sensitive);
    }
    /**
     * @brief Reports the zero-based index position of the last occurrence of a specified string within this instance.
     * @tparam RangeType
     * @param search
     * @param case_sensitive
     * @return
     */
    template<std::ranges::range RangeType>
    size_type last_index_of(const RangeType& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return find_last(search, 0, case_sensitive);
    }

    /**
     * @brief Construct a new UTF-8 string from a UTF-16 string.
     * @param str
     * @return
     */
    NODISCARD static String from_utf16(const char16_t* str)
    {
        return from_utf16(str, std::char_traits<char16_t>::length(str));
    }
    /**
     * @brief Construct a new UTF-8 string from a UTF-16 string.
     * @param str
     * @param length
     * @return
     */
    NODISCARD static String from_utf16(const char16_t* str, size_type length)
    {
        if (length <= 0)
        {
            return {};
        }

        std::string u8 = boost::locale::conv::utf_to_utf<char, char16_t>(str, str + length);
        return {u8.data(), convert_size(u8.length())};
    }
    /**
     * @brief Construct a new UTF-8 string from a UTF-32 string.
     * @param str
     * @return
     */
    NODISCARD static String from_utf32(const char32_t* str)
    {
        return from_utf32(str, std::char_traits<char32_t>::length(str));
    }
    /**
     * @brief Construct a new UTF-8 string from a UTF-32 string.
     * @param str
     * @param length
     * @return
     */
    NODISCARD static String from_utf32(const char32_t* str, size_type length)
    {
        if (length <= 0)
        {
            return {};
        }

        std::string u8 = boost::locale::conv::utf_to_utf<char, char32_t>(str, str + length);
        return {u8.data(), convert_size(u8.length())};
    }
    /**
     * @brief Construct a new UTF-8 string from a std string.
     * @param str
     * @return
     */
    NODISCARD static String from(const std::string& str)
    {
        return {str.data(), convert_size(str.length())};
    }
    /**
     * @brief Construct a new UTF-8 string from a std wstring.
     * @param str
     * @return
     */
    NODISCARD static String from(const std::wstring& str)
    {
        if constexpr (sizeof(std::wstring::value_type) == sizeof(char16_t))
        {
            return String::from_utf16(reinterpret_cast<const char16_t*>(str.data()), convert_size(str.length()));
        }
        else if constexpr (sizeof(std::wstring::value_type) == sizeof(char32_t))
        {
            return String::from_utf32(reinterpret_cast<const char32_t*>(str.data()), convert_size(str.length()));
        }
        ASSERT(0);
    }

    template <typename CharType, typename... Args>
    static String format(const CharType* fmt, Args&&... args)
    {
        static_assert(std::is_same_v<CharType, char> || std::is_same_v<CharType, char8_t>);
        fmt::basic_memory_buffer<CharType, 250> buffer;
        fmt::detail::vformat_to(buffer, fmt::basic_string_view<CharType>(fmt), fmt::make_format_args<fmt::buffer_context<CharType>>(args...));
        return {buffer.data(), convert_size(buffer.size())};
    }
protected:
    allocator_type& get_alloc()              { return pair_.First(); }
    const allocator_type& get_alloc() const  { return pair_.First(); }

    val_type& get_val()                      { return pair_.Second(); }
    const val_type& get_val() const          { return pair_.Second(); }

    void construct(const_pointer str, size_type len);

    void construct(value_type ch, size_type count);

    void construct(const String& right, size_type offset, size_type size);

    void move_construct(String& right, size_type offset, size_type size);

    void assign(const_pointer right, size_type length);

    void move_assign(String& right);

    void eos(size_type size)
    {
        auto&& my_val = get_val();
        my_val.size_ = size;
        char_traits::assign(my_val.get_ptr()[size], value_type());
    }

    bool is_valid_address(const_pointer start, const_pointer end) const;

    size_type calculate_growth(size_type requested) const;

    void reallocate(size_type new_capacity);

    void reallocate_growth(size_type& increase_size);

    template<std::integral T>
    constexpr static size_type convert_size(T size)
    {
        ASSERT(size <= max_size());
        if constexpr (!std::is_same_v<T, size_type>)
        {
            return static_cast<size_type>(size);
        }
        return size;
    }

    CompressionPair<allocator_type, val_type> pair_;
};

using StringView = BasicStringView<String::value_type>;

}

template<>
struct CORE_API fmt::formatter<atlas::String> : formatter<fmt::string_view>
{
    template <typename FormatContext>
    auto format(const atlas::String& str, FormatContext& ctx)
    {
        return formatter<fmt::string_view>::format({str.data(), str.length()}, ctx);
    }
};
