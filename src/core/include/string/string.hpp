// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>

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

    size_type Size() const;

    size_type Length() const;

    NODISCARD size_type Count() const;

    size_type Capacity() const;

    NODISCARD CodePoint CodePointAt(std::make_unsigned_t<size_type> offset) const;

    NODISCARD bool Equals(const String& right, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    void Reserve(size_type capacity);

    NODISCARD bool IsValidIndex(size_type index) const;

    NODISCARD String FoldCase() const;

    NODISCARD String ToUpper(const std::locale& locale = locale::DefaultLocale()) const;

    NODISCARD String ToLower(const std::locale& locale = locale::DefaultLocale()) const;

    String& Append(const String& str);
    String& Append(const char* str);
    String& Append(const char8_t* str);
    String& Append(const view_type& view);
    template<typename IteratorType>
    String& Append(const IteratorType& begin, const IteratorType& end);

    String& Prepend(const String& str);
    String& Prepend(const char* str);
    String& Prepend(const char8_t* str);
    String& Prepend(const view_type& view);
    template<typename IteratorType>
    String& Prepend(const IteratorType& begin, const IteratorType& end);

    NODISCARD String Concat(const String& str);
    NODISCARD String Concat(const char* str);
    NODISCARD String Concat(const char8_t* str);
    NODISCARD String Concat(const view_type& view);
    template<typename IteratorType>
    NODISCARD String Concat(const IteratorType& begin, const IteratorType& end);

    String& Insert(size_type offset, const String& str);
    String& Insert(size_type offset, const char* str);
    String& Insert(size_type offset, const char8_t* str);
    String& Insert(size_type offset, const view_type& view);
    template<typename IteratorType>
    String& Insert(const const_iterator& where, const IteratorType& begin, const IteratorType& end);

    String& Remove(size_type from, size_type count);

    size_type IndexOf(const String& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;
    size_type IndexOf(const char* search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;
    size_type IndexOf(const char8_t* search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;
    size_type IndexOf(const view_type& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;
    template<typename RangeType>
    size_type IndexOf(const RangeType& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    size_type LastIndexOf(const String& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;
    size_type LastIndexOf(const char* search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;
    size_type LastIndexOf(const char8_t* search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;
    size_type LastIndexOf(const view_type& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;
    template<typename RangeType>
    size_type LastIndexOf(const RangeType& search, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

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

inline String::size_type String::Size() const
{
    return GetVal().size_ + 1;
}

inline String::size_type String::Length() const
{
    return GetVal().size_;
}

inline String::size_type String::Capacity() const
{
    return GetVal().capacity_;
}

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

inline String& String::Append(const String& str)
{
    return Append(str.Data(), str.Data() + str.Length());
}

inline String& String::Append(const char* str)
{
    return Append(str, str + std::char_traits<char>::length(str));
}

inline String& String::Append(const char8_t* str)
{
    return Append(str, str + char_traits::length(str));
}

inline String& String::Append(const view_type& view)
{
    return Append(view.begin(), view.end());
}

template<typename IteratorType>
String& String::Append(const IteratorType& begin, const IteratorType& end)
{
    size_type length = Length();
    size_type new_length = length + std::distance(begin, end);
    Reserve(new_length + 1);
    char8_t* p = Data() + length;
    for (auto it = begin; it < end; ++p, ++it)
    {
        char_traits::assign(*p, *it);
    }
    Eos(new_length);

    return *this;
}

inline String& String::Prepend(const String& str)
{
    return Prepend(str.Data(), str.Data() + str.Length());
}

inline String& String::Prepend(const char* str)
{
    return Prepend(str, str + std::char_traits<char>::length(str));
}

inline String& String::Prepend(const char8_t* str)
{
    return Prepend(str, str + char_traits::length(str));
}

inline String& String::Prepend(const view_type& view)
{
    return Prepend(view.begin(), view.end());
}

template<typename IteratorType>
String& String::Prepend(const IteratorType& begin, const IteratorType& end)
{
    size_type length = Length();
    size_type insert_size = std::distance(begin, end);
    size_type new_length = length + insert_size;

    Reserve(new_length + 1);
    char8_t* p = Data();
    char_traits::move(p + insert_size, p, length);

    for (auto it = begin; it < end; ++p, ++it)
    {
        char_traits::assign(*p, *it);
    }
    Eos(new_length);

    return *this;
}

inline String String::Concat(const String& str)
{
    return Concat(str.Data(), str.Data() + str.Length());
}

inline String String::Concat(const char* str)
{
    return Concat(str, str + std::char_traits<char>::length(str));
}

inline String String::Concat(const char8_t* str)
{
    return Concat(str, str + char_traits::length(str));
}

inline String String::Concat(const view_type& view)
{
    return Concat(view.begin(), view.end());
}

template<typename IteratorType>
String String::Concat(const IteratorType& begin, const IteratorType& end)
{
    size_type length = Length();
    size_type new_length = length + std::distance(begin, end);

    String result;
    result.Reserve(new_length + 1);
    char8_t* p = result.Data();

    char_traits::copy(p, Data(), length);
    p += length;
    for (auto it = begin; it < end; ++p, ++it)
    {
        char_traits::assign(*p, *it);
    }
    result.Eos(new_length);

    return result;
}

inline String& String::Insert(size_type offset, const String& str) { return Insert(cbegin() + offset, str.begin(), str.end()); };
inline String& String::Insert(size_type offset, const char* str) { return Insert(cbegin() + offset, str, str + std::char_traits<char>::length(str)); };
inline String& String::Insert(size_type offset, const char8_t* str) { return Insert(cbegin() + offset, str, str + char_traits::length(str)); };
inline String& String::Insert(size_type offset, const view_type& view) { return Insert(cbegin() + offset, view.cbegin(), view.cend()); };

template<typename IteratorType>
String& String::Insert(const const_iterator& where,  const IteratorType& begin, const IteratorType& end)
{
    ASSERT(where >= cbegin() && where <= cend());
    size_type length = Length();
    size_type insert_size = std::distance(begin, end);
    size_type new_length = length + insert_size;
    size_t offset = std::distance(cbegin(), where);
    Reserve(new_length + 1);

    char8_t* p = Data() + offset;

    char_traits::move(p + insert_size, p, length - offset);

    for (auto it = begin; it < end; ++p, ++it)
    {
        char_traits::assign(*p, *it);
    }

    Eos(new_length);

    return *this;
}

inline String::size_type String::IndexOf(const String& search, ECaseSensitive case_sensitive) const
{
    return IndexOf(boost::make_iterator_range(search.begin(), search.end()), case_sensitive);
}

inline String::size_type String::IndexOf(const char* search, ECaseSensitive case_sensitive) const
{
    return IndexOf(boost::make_iterator_range(search, search + std::char_traits<char>::length(search)), case_sensitive);
}

inline String::size_type String::IndexOf(const char8_t* search, ECaseSensitive case_sensitive) const
{
    return IndexOf(boost::make_iterator_range(search, search + char_traits::length(search)), case_sensitive);
}

inline String::size_type String::IndexOf(const view_type& search, ECaseSensitive case_sensitive) const
{
    return IndexOf(boost::make_iterator_range(search.begin(), search.end()), case_sensitive);
}

template<typename RangeType>
String::size_type String::IndexOf(const RangeType& search, ECaseSensitive case_sensitive) const
{
    auto&& range = case_sensitive == ECaseSensitive::Sensitive
        ? boost::algorithm::find_first(*this, search)
        : boost::algorithm::ifind_first(*this, search);

    return range.empty() ? INDEX_NONE_ZU : std::distance(cbegin(), range.begin());
}

inline String::size_type String::LastIndexOf(const String& search, ECaseSensitive case_sensitive) const
{
    return LastIndexOf(boost::make_iterator_range(search.begin(), search.end()), case_sensitive);
}

inline String::size_type String::LastIndexOf(const char* search, ECaseSensitive case_sensitive) const
{
    return LastIndexOf(boost::make_iterator_range(search, search + std::char_traits<char>::length(search)), case_sensitive);
}

inline String::size_type String::LastIndexOf(const char8_t* search, ECaseSensitive case_sensitive) const
{
    return LastIndexOf(boost::make_iterator_range(search, search + char_traits::length(search)), case_sensitive);
}

inline String::size_type String::LastIndexOf(const view_type& search, ECaseSensitive case_sensitive) const
{
    return LastIndexOf(boost::make_iterator_range(search.begin(), search.end()), case_sensitive);
}

template<typename RangeType>
String::size_type String::LastIndexOf(const RangeType& search, ECaseSensitive case_sensitive) const
{
    auto&& range = case_sensitive == ECaseSensitive::Sensitive
                   ? boost::algorithm::find_last(*this, search)
                   : boost::algorithm::ifind_last(*this, search);

    return range.empty() ? INDEX_NONE_ZU : std::distance(cbegin(), range.begin());
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
