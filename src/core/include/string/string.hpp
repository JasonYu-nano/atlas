// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include <string_view>
#include <ranges>

#include "fmt/format.h"
#include "boost/algorithm/string/find.hpp"
#include "boost/algorithm/string/finder.hpp"

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
public:
    CharType operator() (const CharType ch, const std::locale& loc)
    {
        return std::tolower(ch, loc);
    }
};

template<typename CharType>
struct FoldCaseUnsafeImpl<CharType, 1>
{
public:
    CharType operator() (const CharType ch, const std::locale& loc)
    {
        return static_cast<CharType>(std::tolower(static_cast<char>(ch), loc));
    }
};

template<typename CharType>
struct FoldCaseUnsafeImpl<CharType, 2>
{
public:
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
    using char_type = CharTraits::char_type;
public:
    static constexpr size_t INLINE_SIZE = (16 / sizeof(char_type) < 1) ? 1 : 16 / sizeof(char_type);

    bool LargeStringEngaged() const { return INLINE_SIZE <= capacity_; }
    char_type* GetPtr() { return LargeStringEngaged() ? u_.ptr_ : u_.buffer_; }
    const char_type* GetPtr() const { return LargeStringEngaged() ? u_.ptr_ : u_.buffer_; }

    SizeType size_{ 0 };
    SizeType capacity_{ INLINE_SIZE - 1 };

    union UnionBuffer
    {
        char_type buffer_[INLINE_SIZE];
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
    using value_type = char8_t;
    using char_traits = std::char_traits<value_type>;
    using allocator_type = HeapAllocator<value_type>;
    using allocator_traits = AllocatorTraits<allocator_type>;
    using size_type = allocator_traits::size_type;
    using view_type = std::basic_string_view<char8_t>;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = PointerIterator<value_type>;
    using const_iterator = ConstPointerIterator<value_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    using val_type = details::StringVal<char_traits, size_type>;
    using param_type = typename CallTraits<value_type>::param_type;

public:
    String() noexcept { Eos(0); }
    explicit String(value_type ch)
    {
        Construct(ch, 1);
    }
    explicit String(char ch)
    {
        Construct(static_cast<value_type>(ch), 1);
    }

    String(value_type ch, size_type count)
    {
        Construct(ch, count);
    }
    String(char ch, size_type count)
    {
        Construct(static_cast<value_type>(ch), count);
    }

    String(const_pointer str)
    {
        Construct(str, ConvertSize(char_traits::length(str)));
    }
    String(const_pointer str, size_type length)
    {
        Construct(str, length);
    }
    String(const char* str)
    {
        Construct(reinterpret_cast<const_pointer>(str), ConvertSize(std::char_traits<char>::length(str)));
    }
    String(const char* str, size_type length)
    {
        Construct(reinterpret_cast<const_pointer>(str), length);
    }

    String(const String& right)
    {
        Construct(right, 0, MaxSize());
    }
    String(const String& right, size_type offset, size_type size = MaxSize())
    {
        Construct(right, offset, size);
    }
    String(String&& right) noexcept : pair_(OneThenVariadicArgs(), std::move(right.GetAlloc())
        , std::exchange(right.GetVal().size_, 0)
        , std::exchange(right.GetVal().capacity_, 0)
        , std::exchange(right.GetVal().u_, val_type::UnionBuffer()))
    {}
    String(String&& right, size_type offset, size_type size = MaxSize()) noexcept
    {
        MoveConstruct(right, offset, size);
    }

    ~String() noexcept;

    String& operator= (const String& right)
    {
        Assign(right.Data(), right.Length());
        return *this;
    }
    String& operator= (String&& right) noexcept
    {
        MoveAssign(right);
        return *this;
    }
    String& operator= (const char* right)
    {
        Assign(reinterpret_cast<const char8_t*>(right), ConvertSize(std::char_traits<char>::length(right)));
        return *this;
    }
    String& operator= (const char8_t* right)
    {
        Assign(right, char_traits::length(right));
        return *this;
    }

    bool operator== (const String& right) const { return Equals(right, ECaseSensitive::Sensitive); }
    bool operator!= (const String& right) const { return !Equals(right, ECaseSensitive::Sensitive); }
    bool operator< (const String& right) const  { return Compare(right, ECaseSensitive::Sensitive) < 0; }
    bool operator> (const String& right) const  { return Compare(right, ECaseSensitive::Sensitive) > 0; }

    char8_t& operator[] (size_type index)
    {
        ASSERT(IsValidIndex(index));
        return GetVal().GetPtr()[index];
    }
    char8_t operator[] (size_type index) const
    {
        ASSERT(IsValidIndex(index));
        return GetVal().GetPtr()[index];
    }

    NODISCARD inline char8_t* Data()                    { return GetVal().GetPtr(); }
    NODISCARD inline const_pointer Data() const         { return GetVal().GetPtr(); }
    NODISCARD DO_NOT_USE_DIRECTLY inline char8_t* data()                    { return GetVal().GetPtr(); }
    NODISCARD DO_NOT_USE_DIRECTLY inline const_pointer data() const         { return GetVal().GetPtr(); }

    NODISCARD iterator begin()                          { return iterator(Data()); }
    NODISCARD const_iterator begin() const              { return const_iterator(Data()); }
    NODISCARD iterator end()                            { return iterator(Data() + Length()); }
    NODISCARD const_iterator end() const                { return const_iterator(Data() + Length()); }

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
    size_type Length() const { return GetVal().size_; }
    /**
     * @brief Gets the number of characters in the current string.
     * @return
     */
    size_type Size() const { return GetVal().size_; }
    /**
     * @brief To implement std::sized_range, avoid direct usage.
     * @return
     */
    DO_NOT_USE_DIRECTLY size_type size() const { return Size(); }

    DO_NOT_USE_DIRECTLY constexpr size_type max_size() { return MaxSize(); }

    /**
     * @brief Get the number of code points contained in the string.
     * @return
     */
    NODISCARD size_type Count() const;

    /**
     * @brief Get the capacity of the current string.
     * @return
     */
    size_type Capacity() const { return GetVal().capacity_; }

    /**
     * @brief Get code point by offset.
     * @param offset
     * @return
     */
    NODISCARD CodePoint CodePointAt(std::make_unsigned_t<size_type> offset) const;

    /**
     * @brief Determines whether this instance and another specified string have the same value.
     * @param right
     * @param case_sensitive
     * @return
     */
    NODISCARD bool Equals(const String& right, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return Compare(right, case_sensitive) == 0;
    }

    /**
     * @brief Compare with another string.
     * @param right
     * @param case_sensitive
     * @return Positive number if larger than another string.
     */
    NODISCARD int32 Compare(const String& right, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    /**
     * @brief If new capacity is greater than the current capacity, new storage is allocated, and capacity is made equal or greater than new capacity.
     * @param capacity
     */
    void Reserve(size_type capacity)
    {
        if (capacity > GetVal().capacity_)
        {
            Reallocate(capacity);
        }
    }

    NODISCARD bool IsValidIndex(size_type index) const { return index < Length(); }

    /**
     * @brief Fold the current string.
     * @param locale
     * @return A new string.
     */
    NODISCARD String FoldCase() const;

    /**
     * @brief Check if the string is uppercase in specified locale.
     * @param locale
     * @return
     */
    NODISCARD bool IsUpper(const std::locale& locale = locale::DefaultLocale()) const;
    /**
     * @brief Translate the current string to uppercase.
     * @param locale
     * @return A new uppercase string.
     */
    NODISCARD String ToUpper(const std::locale& locale = locale::DefaultLocale()) const;
    /**
     * @brief Check if the string is lowercase in specified locale.
     * @param locale
     * @return
     */
    NODISCARD bool IsLower(const std::locale& locale = locale::DefaultLocale()) const;
    /**
     * @brief Translate the current string to lowercase.
     * @param locale
     * @return A new lowercase string.
     */
    NODISCARD String ToLower(const std::locale& locale = locale::DefaultLocale()) const;

    /**
     * @brief Append the specified string to the current string.
     * @param str
     * @return Current string
     */
    String& Append(const char* str) { return Append(std::string_view(str)); }
    /**
     * @brief Append the specified string to the current string.
     * @param str
     * @return Current string
     */
    String& Append(const_pointer str)  { return Append(view_type(str)); }
    /**
     * @brief Append the specified range to the current string.
     * @tparam RangeType
     * @param range
     * @return Current string
     */
    template<std::ranges::contiguous_range RangeType>
    String& Append(const RangeType& range);

    /**
     * @brief Prepend the specified string to the current string.
     * @param str
     * @return Current string
     */
    String& Prepend(const char* str) { return Prepend(std::string_view(str)); }
    /**
     * @brief Prepend the specified string to the current string.
     * @param str
     * @return Current string
     */
    String& Prepend(const_pointer str) { return Prepend(view_type(str)); }
    /**
     * @brief Prepend the specified range to the current string.
     * @tparam RangeType
     * @param range
     * @return Current string
     */
    template<std::ranges::contiguous_range RangeType>
    String& Prepend(const RangeType& range);

    /**
     * @brief Concatenates two specified const string.
     * @param str
     * @return A new string
     */
    NODISCARD String Concat(const char* str) const { return Concat(std::string_view(str)); }
    /**
     * @brief Concatenates two specified const string.
     * @param str
     * @return A new string
     */
    NODISCARD String Concat(const_pointer str) const { return Concat(view_type(str)); }
    /**
     * @brief Concatenate the current string with the specified range.
     * @tparam RangeType
     * @param range
     * @return A new string
     */
    template<std::ranges::contiguous_range RangeType>
    NODISCARD String Concat(const RangeType& range) const;

    /**
     * @brief Insert the specified string into the current string.
     * @param offset
     * @param str
     * @return Current string
     */
    String& Insert(size_type offset, const char* str) { return Insert(cbegin() + offset, std::string_view(str)); }
    /**
     * @brief Insert the specified string into the current string.
     * @param offset
     * @param str
     * @return Current string
     */
    String& Insert(size_type offset, const_pointer str) { return Insert(cbegin() + offset, view_type(str)); }
    /**
     * @brief Insert the specified range into the current string.
     * @tparam RangeType
     * @param where
     * @param range
     * @return Current string
     */
    template<std::ranges::contiguous_range RangeType>
    String& Insert(const const_iterator& where, const RangeType& range);

    /**
     * @brief Remove a specified number of characters from the specified position.
     * @param from
     * @param count
     * @return Current string
     */
    String& Remove(size_type from, size_type count);
    /**
     * @brief Remove all specified strings from the current string.
     * @param str
     * @param case_sensitive
     * @return
     */
    String& Remove(const char* str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive)
    {
        return Remove(std::string_view(str), case_sensitive);
    }
    /**
     * @brief Remove all specified strings from the current string.
     * @param str
     * @param case_sensitive
     * @return
     */
    String& Remove(const_pointer str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive)
    {
        return Remove(view_type(str), case_sensitive);
    }
    /**
     * @brief Remove all specified ranges from the current string.
     * @tparam RangeType
     * @param range
     * @param case_sensitive
     * @return
     */
    template<std::ranges::sized_range RangeType>
    String& Remove(const RangeType& range, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive);

    /**
     * @brief Replace specified number of characters in the current instance with another specified string.
     * @param from
     * @param count
     * @param new_value
     * @return
     */
    String& Replace(size_type from, size_type count, const char* new_value)
    {
        return Replace(from, count, std::string_view(new_value));
    }
    /**
     * @brief Replace specified number of characters in the current instance with another specified string.
     * @param from
     * @param count
     * @param new_value
     * @return
     */
    String& Replace(size_type from, size_type count, const_pointer new_value)
    {
        return Replace(from, count, view_type(new_value));
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
    String& Replace(size_type from, size_type count, const RangeType& new_value);
    /**
     * @brief Replace all occurrences of a specified string in the current instance with another specified string.
     * @param old_value
     * @param new_value
     * @return
     */
    String& Replace(const char* old_value, const char* new_value)
    {
        return Replace(std::string_view(old_value), std::string_view(new_value));
    }
    /**
     * @brief Replace all occurrences of a specified string in the current instance with another specified string.
     * @param old_value
     * @param new_value
     * @return
     */
    String& Replace(const_pointer old_value, const_pointer new_value)
    {
        return Replace(view_type(old_value), view_type(new_value));
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
    String& Replace(const RangeType1& old_value, const RangeType2& new_value, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive);

    /**
     * @brief Determines whether the beginning of this string instance matches the specified string.
     * @param str
     * @param case_sensitive
     * @return
     */
    bool StartsWith(const char* str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return StartsWith(std::string_view(str), case_sensitive);
    }
    /**
     * @brief Determines whether the beginning of this string instance matches the specified string.
     * @param str
     * @param case_sensitive
     * @return
     */
    bool StartsWith(const_pointer str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return StartsWith(view_type(str), case_sensitive);
    }
    /**
     * @brief Determines whether the beginning of this string instance matches the specified range.
     * @tparam RangeType
     * @param range
     * @param case_sensitive
     * @return
     */
    template<std::ranges::sized_range RangeType>
    bool StartsWith(const RangeType& range, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;
    /**
     * @brief Determines whether the end of this string instance matches the specified string.
     * @param str
     * @param case_sensitive
     * @return
     */
    bool EndsWith(const char* str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return EndsWith(std::string_view(str), case_sensitive);
    }
    /**
     * @brief Determines whether the end of this string instance matches the specified string.
     * @param str
     * @param case_sensitive
     * @return
     */
    bool EndsWith(const_pointer str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return EndsWith(view_type(str), case_sensitive);
    }
    /**
     * @brief Determines whether the end of this string instance matches the specified range.
     * @tparam RangeType
     * @param range
     * @param case_sensitive
     * @return
     */
    template<std::ranges::sized_range RangeType>
    bool EndsWith(const RangeType& range, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    template<std::ranges::range RangeType>
    size_type Find(const RangeType& search, size_type offset, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    template<std::ranges::range RangeType>
    size_type FindLast(const RangeType& search, size_type offset_to_tail, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    /**
     * @brief Reports the zero-based index of the first occurrence of the specified string in this instance.
     * @param search
     * @param case_sensitive
     * @return
     */
    size_type IndexOf(const char* search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return IndexOf(std::string_view(search), case_sensitive);
    }
    /**
     * @brief Reports the zero-based index of the first occurrence of the specified string in this instance.
     * @param search
     * @param case_sensitive
     * @return
     */
    size_type IndexOf(const_pointer search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return IndexOf(view_type(search), case_sensitive);
    }
    /**
     * @brief Reports the zero-based index of the first occurrence of the specified string in this instance.
     * @tparam RangeType
     * @param search
     * @param case_sensitive
     * @return
     */
    template<std::ranges::range RangeType>
    size_type IndexOf(const RangeType& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return Find(search, 0, case_sensitive);
    }
    /**
     * @brief Reports the zero-based index position of the last occurrence of a specified string within this instance.
     * @param search
     * @param case_sensitive
     * @return
     */
    size_type LastIndexOf(const char* search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return LastIndexOf(std::string_view(search), case_sensitive);
    }
    /**
     * @brief Reports the zero-based index position of the last occurrence of a specified string within this instance.
     * @param search
     * @param case_sensitive
     * @return
     */
    size_type LastIndexOf(const_pointer search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return LastIndexOf(view_type(search), case_sensitive);
    }
    /**
     * @brief Reports the zero-based index position of the last occurrence of a specified string within this instance.
     * @tparam RangeType
     * @param search
     * @param case_sensitive
     * @return
     */
    template<std::ranges::range RangeType>
    size_type LastIndexOf(const RangeType& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return FindLast(search, 0, case_sensitive);
    }

    constexpr static size_type MaxSize() { return std::numeric_limits<size_type>::max() - 1; }

    /**
     * @brief Construct a new UTF-8 string from a UTF-16 string.
     * @param str
     * @return
     */
    NODISCARD static String FromUtf16(const char16_t* str);
    /**
     * @brief Construct a new UTF-8 string from a UTF-16 string.
     * @param str
     * @param length
     * @return
     */
    NODISCARD static String FromUtf16(const char16_t* str, size_type length);
    /**
     * @brief Construct a new UTF-8 string from a UTF-32 string.
     * @param str
     * @return
     */
    NODISCARD static String FromUtf32(const char32_t* str);
    /**
     * @brief Construct a new UTF-8 string from a UTF-32 string.
     * @param str
     * @param length
     * @return
     */
    NODISCARD static String FromUtf32(const char32_t* str, size_type length);
    /**
     * @brief Construct a new UTF-8 string from a std string.
     * @param str
     * @return
     */
    NODISCARD static String From(const std::string& str);
    /**
     * @brief Construct a new UTF-8 string from a std wstring.
     * @param str
     * @return
     */
    NODISCARD static String From(const std::wstring& str);

    template <typename CharType, typename... Args>
    static String Format(const CharType* fmt, Args&&... args)
    {
        static_assert(std::is_same_v<CharType, char> || std::is_same_v<CharType, char8_t>);
        fmt::basic_memory_buffer<CharType, 250> buffer;
        fmt::detail::vformat_to(buffer, fmt::basic_string_view<CharType>(fmt), fmt::make_format_args<fmt::buffer_context<CharType>>(args...));
        return {buffer.data(), ConvertSize(buffer.size())};
    }
protected:
    allocator_type& GetAlloc()              { return pair_.First(); }
    const allocator_type& GetAlloc() const  { return pair_.First(); }

    val_type& GetVal()                      { return pair_.Second(); }
    const val_type& GetVal() const          { return pair_.Second(); }

    void Construct(const_pointer str, size_type len);
    void Construct(char8_t ch, size_type count);
    void Construct(const String& right, size_type offset, size_type size);

    void MoveConstruct(String& right, size_type offset, size_type size);

    void Assign(const char8_t* right, size_type length);

    void MoveAssign(String& right);

    void Eos(size_type size);

    bool IsValidAddress(const char8_t* start, const char8_t* end) const;

    size_type CalculateGrowth(size_type requested) const;

    void Reallocate(size_type new_capacity);
    void ReallocateGrowth(size_type& increase_size);

    template<std::integral T>
    constexpr static size_type ConvertSize(T size)
    {
        ASSERT(size <= MaxSize());
        if constexpr (!std::is_same_v<T, size_type>)
        {
            return static_cast<size_type>(size);
        }
        return size;
    }

    CompressionPair<allocator_type, val_type> pair_;
};

inline String::~String() noexcept
{
    auto&& my_val = GetVal();
    if (my_val.LargeStringEngaged())
    {
        allocator_traits::deallocate(GetAlloc(), my_val.GetPtr(), my_val.capacity_ + 1);
    }
    my_val.size_ = 0;
    my_val.capacity_ = val_type::INLINE_SIZE - 1;
}

inline String String::FromUtf16(const char16_t* str)
{
    return FromUtf16(str, std::char_traits<char16_t>::length(str));
}

inline String String::FromUtf32(const char32_t* str)
{
    return FromUtf32(str, std::char_traits<char32_t>::length(str));
}

inline String String::From(const std::string& str)
{
    return {str.data(), ConvertSize(str.length())};
}

inline String String::From(const std::wstring& str)
{
    if constexpr (sizeof(std::wstring::value_type) == sizeof(char16_t))
    {
        return String::FromUtf16(reinterpret_cast<const char16_t*>(str.data()), ConvertSize(str.length()));
    }
    else if constexpr (sizeof(std::wstring::value_type) == sizeof(char32_t))
    {
        return String::FromUtf32(reinterpret_cast<const char32_t*>(str.data()), ConvertSize(str.length()));
    }
    ASSERT(0);
}

template<std::ranges::contiguous_range RangeType>
String& String::Append(const RangeType& range)
{
    size_type length = Length();
    size_type increase_size = ConvertSize(range.size());
    auto&& my_val = GetVal();
    if (increase_size > my_val.capacity_ - my_val.size_)
    {
        ReallocateGrowth(increase_size);
    }

    char_traits::copy(my_val.GetPtr() + length, reinterpret_cast<const_pointer>(range.data()), increase_size);
    return *this;
}

template<std::ranges::contiguous_range RangeType>
String& String::Prepend(const RangeType& range)
{
    size_type length = Length();
    size_type increase_size = ConvertSize(range.size());
    auto&& my_val = GetVal();
    if (increase_size > my_val.capacity_ - my_val.size_)
    {
        ReallocateGrowth(increase_size);
    }

    char8_t* ptr = my_val.GetPtr();
    char_traits::move(ptr + increase_size, ptr, length);
    char_traits::copy(ptr, reinterpret_cast<const_pointer>(range.data()), increase_size);
    return *this;
}

template<std::ranges::contiguous_range RangeType>
String String::Concat(const RangeType& range) const
{
    size_type length = Length();
    size_type increase_size = ConvertSize(range.size());
    if (increase_size > MaxSize() - length)
    {
        ASSERT(0);
        increase_size = MaxSize() - length;
    }

    size_type new_length = length + increase_size;

    String result;
    result.Reserve(new_length);

    char8_t* ptr = result.Data();
    char_traits::copy(ptr, Data(), length);
    char_traits::copy(ptr + length, reinterpret_cast<const_pointer>(range.data()), increase_size);
    result.Eos(new_length);

    return result;
}

template<std::ranges::contiguous_range RangeType>
String& String::Insert(const const_iterator& where, const RangeType& range)
{
    ASSERT(where >= cbegin() && where <= cend());
    auto&& my_val = GetVal();
    size_type increase_size = ConvertSize(range.size());
    size_type offset = ConvertSize(std::distance(cbegin(), where));
    size_type length = my_val.size_;

    if (increase_size > my_val.capacity_ - my_val.size_)
    {
        ReallocateGrowth(increase_size);
    }

    char8_t* ptr = my_val.GetPtr() + offset;
    char_traits::move(ptr + increase_size, ptr, length - offset);
    char_traits::copy(ptr, reinterpret_cast<const_pointer>(range.data()), increase_size);

    return *this;
}

template<std::ranges::sized_range RangeType>
String& String::Remove(const RangeType& range, ECaseSensitive case_sensitive)
{
    size_type index = 0;
    size_type size = ConvertSize(range.size());
    do
    {
        index = Find(range, index, case_sensitive);
        if (index == INDEX_NONE)
        {
            break;
        }

        Remove(index, size);

    } while (true);
    return *this;
}

template<std::ranges::contiguous_range RangeType>
String& String::Replace(size_type from, size_type count, const RangeType& new_value)
{
    ASSERT(count > 0 && from < Length() && from <= Length() - count);

    size_type length = Length();
    size_type insert_size = ConvertSize(new_value.size());
    if (insert_size > count)
    {
        size_type increase_size = insert_size - count;
        if (increase_size > MaxSize() - length)
        {
            ASSERT(0);
            insert_size = MaxSize() - length + count;
        }
    }
    size_type new_length = length - count + insert_size;
    Reserve(new_length);

    pointer start = Data() + from;
    char_traits::move(start + insert_size, start + count, length - from - count);
    char_traits::copy(start, reinterpret_cast<const_pointer>(new_value.data()), insert_size);

    Eos(new_length);

    return *this;
}

template<std::ranges::sized_range RangeType1, std::ranges::contiguous_range RangeType2>
String& String::Replace(const RangeType1& old_value, const RangeType2& new_value, ECaseSensitive case_sensitive)
{
    size_type index = 0;
    size_type size = old_value.size();
    do
    {
        index = Find(old_value, index, case_sensitive);
        if (index == INDEX_NONE)
        {
            break;
        }

        Replace(index, size, new_value);

    } while (true);
    return *this;
}

template<std::ranges::sized_range RangeType>
bool String::StartsWith(const RangeType& range, ECaseSensitive case_sensitive) const
{
    if (Length() < range.size())
    {
        return false;
    }

    const_iterator p = cbegin();
    const std::locale& loc = locale::DefaultLocale();
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

template<std::ranges::sized_range RangeType>
bool String::EndsWith(const RangeType& range, ECaseSensitive case_sensitive) const
{
    if (Length() < range.size())
    {
        return false;
    }

    const_reverse_iterator p = crbegin();
    const std::locale& loc = locale::DefaultLocale();
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
String::size_type String::Find(const RangeType& search, size_type offset, ECaseSensitive case_sensitive) const
{
    auto&& source = boost::make_iterator_range(begin() + offset, end());
    auto&& range = case_sensitive == ECaseSensitive::Sensitive
                   ? boost::algorithm::find_first(source, search)
                   : boost::algorithm::find(source, ::boost::algorithm::first_finder(search, details::EqualsInsensitive(locale::DefaultLocale())));

    return range.empty() ? size_type(INDEX_NONE) : std::distance(cbegin(), range.begin());
}

template<std::ranges::range RangeType>
String::size_type String::FindLast(const RangeType& search, size_type offset_to_tail, ECaseSensitive case_sensitive) const
{
    auto&& source = boost::make_iterator_range(begin(), end() - offset_to_tail);
    auto&& range = case_sensitive == ECaseSensitive::Sensitive
                   ? boost::algorithm::find_last(source, search)
                   : boost::algorithm::find(source, ::boost::algorithm::last_finder(search, details::EqualsInsensitive(locale::DefaultLocale())));

    return range.empty() ? size_type(INDEX_NONE) : std::distance(cbegin(), range.begin());
}

inline void String::Eos(String::size_type size)
{
    auto&& my_val = GetVal();
    my_val.size_ = size;
    char_traits::assign(my_val.GetPtr()[size], value_type());
}

inline String::size_type String::CalculateGrowth(size_type requested) const
{
    if (requested > MaxSize())
    {
        return MaxSize();
    }

    size_type old = GetVal().capacity_;
    if (old > MaxSize() - old / 2)
    {
        return MaxSize();
    }

    return math::Max(requested, old + old / 2);
}

}

template<>
struct CORE_API fmt::formatter<atlas::String>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const atlas::String& str, FormatContext& ctx)
    {
        auto&& out = ctx.out();
        auto&& buf = fmt::detail::get_buffer<char8_t>(out);
        fmt::detail::vformat_to<char8_t>(buf, str.Data(), {}, {});
        return fmt::detail::get_iterator(buf, out);
    }
};
