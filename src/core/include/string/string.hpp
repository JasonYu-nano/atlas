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
    using Super = FoldCaseUnsafeImpl<CharType, FoldCaseUnsafeSwitcher<CharType>::value>;
public:
    using Super::operator();
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

    union
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

class CORE_API String
{
public:
    using value_type = char8_t;
    using char_traits = std::char_traits<value_type>;
    using allocator_type = StandardAllocator<size_t>::Allocator<value_type>;
    using allocator_traits = std::allocator_traits<allocator_type>;
    using size_type = allocator_traits::size_type;
    using view_type = std::basic_string_view<char8_t>;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = WrapIterator<pointer>;
    using const_iterator = WrapIterator<const_pointer>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    using val_type = details::StringVal<char_traits, size_type>;
    using param_type = typename CallTraits<value_type>::param_type;

public:
    String();
    explicit String(char8_t ch);
    explicit String(char ch);

    String(char8_t ch, size_type count);
    String(char ch, size_type count);

    String(const char8_t* str);
    String(const char8_t* str, size_type length);
    String(const char* str);
    String(const char* str, size_type length);

    String(const String& right);
    String(const String& right, size_type offset, size_type size = std::numeric_limits<size_type>::max());
    String(String&& right) noexcept;
    String(String&& right, size_type offset, size_type size = std::numeric_limits<size_type>::max()) noexcept;

    ~String();

    String& operator= (const String& right);
    String& operator= (String&& right) noexcept;
    String& operator= (const char* right);
    String& operator= (const char8_t* right);

    bool operator== (const String& right) const;
    bool operator!= (const String& right) const;

    char8_t& operator[] (size_type index);
    char8_t operator[] (size_type index) const;

    NODISCARD inline char8_t* Data();
    NODISCARD inline const char8_t* Data() const;

    NODISCARD iterator begin();
    NODISCARD const_iterator begin() const;
    NODISCARD iterator end();
    NODISCARD const_iterator end() const;

    NODISCARD reverse_iterator rbegin();
    NODISCARD const_reverse_iterator rbegin() const;
    NODISCARD reverse_iterator rend();
    NODISCARD const_reverse_iterator rend() const;

    NODISCARD const_iterator cbegin() const;
    NODISCARD const_iterator cend() const;
    NODISCARD const_reverse_iterator crbegin() const;
    NODISCARD const_reverse_iterator crend() const;

    size_type Length() const { return GetVal().size_; }
    size_type Size() const { return GetVal().size_; }
    size_type size() const { return Size(); }

    NODISCARD size_type Count() const;

    size_type Capacity() const { return GetVal().capacity_; }

    NODISCARD CodePoint CodePointAt(std::make_unsigned_t<size_type> offset) const;

    NODISCARD bool Equals(const String& right, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    void Reserve(size_type capacity);

    NODISCARD bool IsValidIndex(size_type index) const;

    NODISCARD String FoldCase() const;

    NODISCARD bool IsUpper(const std::locale& locale = locale::DefaultLocale()) const;
    NODISCARD String ToUpper(const std::locale& locale = locale::DefaultLocale()) const;
    NODISCARD bool IsLower(const std::locale& locale = locale::DefaultLocale()) const;
    NODISCARD String ToLower(const std::locale& locale = locale::DefaultLocale()) const;

    String& Append(const char* str) { return Append(std::string_view(str)); }
    String& Append(const_pointer str)  { return Append(view_type(str)); }
    template<std::ranges::sized_range RangeType>
    String& Append(const RangeType& range);

    String& Prepend(const char* str) { return Prepend(std::string_view(str)); }
    String& Prepend(const_pointer str) { return Prepend(view_type(str)); }
    template<std::ranges::sized_range RangeType>
    String& Prepend(const RangeType& range);

    NODISCARD String Concat(const char* str) { return Concat(std::string_view(str)); }
    NODISCARD String Concat(const_pointer str) { return Concat(view_type(str)); }
    template<std::ranges::sized_range RangeType>
    NODISCARD String Concat(const RangeType& range);

    String& Insert(size_type offset, const char* str) { return Insert(cbegin() + offset, std::string_view(str)); }
    String& Insert(size_type offset, const_pointer str) { return Insert(cbegin() + offset, view_type(str)); }
    template<std::ranges::sized_range RangeType>
    String& Insert(const const_iterator& where, const RangeType& range);

    String& Remove(size_type from, size_type count);
    String& Remove(const char* str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive)
    {
        return Remove(std::string_view(str), case_sensitive);
    }

    String& Remove(const_pointer str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive)
    {
        return Remove(view_type(str), case_sensitive);
    }

    template<std::ranges::sized_range RangeType>
    String& Remove(const RangeType& range, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive);

    bool StartsWith(const char* str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return StartsWith(std::string_view(str), case_sensitive);
    }

    bool StartsWith(const_pointer str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return StartsWith(view_type(str), case_sensitive);
    }

    template<std::ranges::sized_range RangeType>
    bool StartsWith(const RangeType& range, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    bool EndsWith(const char* str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return EndsWith(std::string_view(str), case_sensitive);
    }

    bool EndsWith(const_pointer str, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return EndsWith(view_type(str), case_sensitive);
    }

    template<std::ranges::sized_range RangeType>
    bool EndsWith(const RangeType& range, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    template<std::ranges::range RangeType>
    size_type Find(const RangeType& search, size_type offset, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    template<std::ranges::range RangeType>
    size_type FindLast(const RangeType& search, size_type offset, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    size_type IndexOf(const char* search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return IndexOf(std::string_view(search), case_sensitive);
    }

    size_type IndexOf(const_pointer search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return IndexOf(view_type(search), case_sensitive);
    }

    template<std::ranges::range RangeType>
    size_type IndexOf(const RangeType& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return Find(search, 0, case_sensitive);
    }

    size_type LastIndexOf(const char* search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return LastIndexOf(std::string_view(search), case_sensitive);
    }

    size_type LastIndexOf(const_pointer search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return LastIndexOf(view_type(search), case_sensitive);
    }

    template<std::ranges::range RangeType>
    size_type LastIndexOf(const RangeType& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const
    {
        return FindLast(search, 0, case_sensitive);
    }

    NODISCARD static String FromUtf16(const char16_t* str);
    NODISCARD static String FromUtf16(const char16_t* str, size_type length);

    NODISCARD static String FromUtf32(const char32_t* str);
    NODISCARD static String FromUtf32(const char32_t* str, size_type length);

    NODISCARD static String From(const std::string& str);
    NODISCARD static String From(const std::wstring& str);

    template <typename CharType, typename... Args>
    static String Format(const CharType* fmt, Args&&... args)
    {
        static_assert(std::is_same_v<CharType, char> || std::is_same_v<CharType, char8_t>);
        fmt::basic_memory_buffer<CharType, 250> buffer;
        fmt::detail::vformat_to(buffer, fmt::basic_string_view<CharType>(fmt), fmt::make_format_args<fmt::buffer_context<CharType>>(args...));
        return {buffer.data(), static_cast<size_type>(buffer.size())};
    }
protected:
    bool LargeStringEngaged() const { return GetVal().LargeStringEngaged(); }

    allocator_type& GetAlloc() { return pair_.First(); }
    const allocator_type& GetAlloc() const { return pair_.First(); }

    val_type& GetVal() { return pair_.Second(); }
    const val_type& GetVal() const { return pair_.Second(); }

    void BecomeLarge(size_type capacity);

    void TidyInit();

    void Construct(const_pointer str, size_type len);
    void Construct(char8_t ch, size_type count);
    void Construct(const String& right, size_type offset, size_type size);

    void MoveConstruct(String& right, size_type offset, size_type size);

    void Assign(const char8_t* right, size_type length);

    void MoveAssign(String& right);

    void Eos(size_type size);

    bool IsValidAddress(const char8_t* start, const char8_t* end) const;

    CompressionPair<allocator_type, val_type> pair_;
};

inline char8_t& String::operator[] (size_type index)
{
    ASSERT(IsValidIndex(index));
    return GetVal().GetPtr()[index];
}

inline char8_t String::operator[] (size_type index) const
{
    ASSERT(IsValidIndex(index));
    return GetVal().GetPtr()[index];
}

inline char8_t* String::Data()
{
    return GetVal().GetPtr();
}

inline const char8_t* String::Data() const
{
    return GetVal().GetPtr();
}

inline String::iterator String::begin() { return iterator(Data()); }

inline String::const_iterator String::begin() const{ return const_iterator(Data()); }

inline String::iterator String::end() { return iterator(Data() + Length()); }

inline String::const_iterator String::end() const { return const_iterator(Data() + Length()); }

inline String::reverse_iterator String::rbegin() { return reverse_iterator(end()); }

inline String::const_reverse_iterator String::rbegin() const{ return const_reverse_iterator(end()); }

inline String::reverse_iterator String::rend() { return reverse_iterator(begin()); }

inline String::const_reverse_iterator String::rend() const { return const_reverse_iterator(begin()); }

inline String::const_iterator String::cbegin() const { return begin(); }

inline String::const_iterator String::cend() const { return end(); }

inline String::const_reverse_iterator String::crbegin() const{ return const_reverse_iterator(end()); }

inline String::const_reverse_iterator String::crend() const { return const_reverse_iterator(begin()); }

inline bool String::IsValidIndex(String::size_type index) const
{
    return index < Length();
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
    return {str.data(), static_cast<size_type>(str.length())};
}

inline String String::From(const std::wstring& str)
{
    if constexpr (sizeof(std::wstring::value_type) == sizeof(char16_t))
    {
        return String::FromUtf16(reinterpret_cast<const char16_t*>(str.data()), static_cast<size_type>(str.length()));
    }
    else if constexpr (sizeof(std::wstring::value_type) == sizeof(char32_t))
    {
        return String::FromUtf32(reinterpret_cast<const char32_t*>(str.data()), static_cast<size_type>(str.length()));
    }
    ASSERT(0);
}

template<std::ranges::sized_range RangeType>
String& String::Append(const RangeType& range)
{
    size_type length = Length();
    size_type new_length = length + range.size();
    Reserve(new_length + 1);
    char8_t* p = Data() + length;
    for (auto it = range.begin(); it < range.end(); ++p, ++it)
    {
        char_traits::assign(*p, *it);
    }
    Eos(new_length);

    return *this;
}

template<std::ranges::sized_range RangeType>
String& String::Prepend(const RangeType& range)
{
    size_type length = Length();
    size_type insert_size = range.size();
    size_type new_length = length + insert_size;

    Reserve(new_length + 1);
    char8_t* p = Data();
    char_traits::move(p + insert_size, p, length);

    for (auto it = range.begin(); it < range.end(); ++p, ++it)
    {
        char_traits::assign(*p, *it);
    }
    Eos(new_length);

    return *this;
}

template<std::ranges::sized_range RangeType>
String String::Concat(const RangeType& range)
{
    size_type length = Length();
    size_type new_length = length + range.size();

    String result;
    result.Reserve(new_length + 1);
    char8_t* p = result.Data();

    char_traits::copy(p, Data(), length);
    p += length;
    for (auto it = range.begin(); it < range.end(); ++p, ++it)
    {
        char_traits::assign(*p, *it);
    }
    result.Eos(new_length);

    return result;
}

template<std::ranges::sized_range RangeType>
String& String::Insert(const const_iterator& where, const RangeType& range)
{
    ASSERT(where >= cbegin() && where <= cend());
    size_type length = Length();
    size_type insert_size = range.size();
    size_type new_length = length + insert_size;
    size_t offset = std::distance(cbegin(), where);
    Reserve(new_length + 1);

    char8_t* p = Data() + offset;

    char_traits::move(p + insert_size, p, length - offset);

    for (auto it = range.begin(); it < range.end(); ++p, ++it)
    {
        char_traits::assign(*p, *it);
    }

    Eos(new_length);

    return *this;
}

template<std::ranges::sized_range RangeType>
String& String::Remove(const RangeType& range, ECaseSensitive case_sensitive)
{
    size_type index = 0;
    size_type size = range.size();
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
String::size_type String::FindLast(const RangeType& search, size_type offset, ECaseSensitive case_sensitive) const
{
    auto&& source = boost::make_iterator_range(begin(), end() - offset);
    auto&& range = case_sensitive == ECaseSensitive::Sensitive
                   ? boost::algorithm::find_last(source, search)
                   : boost::algorithm::find(source, ::boost::algorithm::last_finder(search, details::EqualsInsensitive(locale::DefaultLocale())));

    return range.empty() ? size_type(INDEX_NONE) : std::distance(cbegin(), range.begin());
}

inline void String::TidyInit()
{
    Eos(0);
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
