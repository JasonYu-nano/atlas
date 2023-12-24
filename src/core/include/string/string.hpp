// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>

#include "fmt/format.h"

#include "utility/compression_pair.hpp"
#include "memory/allocator.hpp"
#include "string/locale.hpp"
#include "string/unicode.hpp"
#include "misc/iterator.hpp"

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
    using AllocatorType = StandardAllocator<size_t>::Allocator<ValueType>;
    using AllocatorTraits = std::allocator_traits<AllocatorType>;
    using SizeType = AllocatorTraits::size_type;
    using ViewType = std::basic_string_view<char8_t>;
    using Pointer = ValueType*;
    using ConstPointer = const ValueType*;
    using Iterator = WrapIterator<Pointer>;
    using ConstIterator = WrapIterator<ConstPointer>;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

private:
    using ValType = details::StringVal<CharTraits, SizeType>;
    using ParamType = typename CallTraits<ValueType>::ParamType;

public:
    String();
    explicit String(char8_t ch);
    explicit String(char ch);

    String(char8_t ch, SizeType count);
    String(char ch, SizeType count);

    String(const char8_t* str);
    String(const char8_t* str, SizeType length);
    String(const char* str);
    String(const char* str, SizeType length);

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

    char8_t& operator[] (SizeType index);
    char8_t operator[] (SizeType index) const;

    NODISCARD inline char8_t* Data();
    NODISCARD inline const char8_t* Data() const;

    NODISCARD Iterator begin();
    NODISCARD ConstIterator begin() const;
    NODISCARD Iterator end();
    NODISCARD ConstIterator end() const;

    NODISCARD ReverseIterator rbegin();
    NODISCARD ConstReverseIterator rbegin() const;
    NODISCARD ReverseIterator rend();
    NODISCARD ConstReverseIterator rend() const;

    NODISCARD ConstIterator cbegin() const;
    NODISCARD ConstIterator cend() const;
    NODISCARD ConstReverseIterator crbegin() const;
    NODISCARD ConstReverseIterator crend() const;

    SizeType Size() const;

    SizeType Length() const;

    NODISCARD SizeType Count() const;

    SizeType Capacity() const;

    NODISCARD CodePoint CodePointAt(std::make_unsigned_t<SizeType> offset) const;

    NODISCARD bool Equals(const String& right, ECaseSensitive case_sensitive = ECaseSensitive::Sensitive) const;

    void Reserve(SizeType capacity);

    NODISCARD bool IsValidIndex(SizeType index) const;

    NODISCARD String FoldCase() const;

    NODISCARD String ToUpper(const std::locale& locale = locale::DefaultLocale()) const;

    NODISCARD String ToLower(const std::locale& locale = locale::DefaultLocale()) const;

    String& Append(const String& str);
    String& Append(const char* str);
    String& Append(const char8_t* str);
    String& Append(const ViewType& view);
    template<typename IteratorType>
    String& Append(const IteratorType& begin, const IteratorType& end);

    String& Prepend(const String& str);
    String& Prepend(const char* str);
    String& Prepend(const char8_t* str);
    String& Prepend(const ViewType& view);
    template<typename IteratorType>
    String& Prepend(const IteratorType& begin, const IteratorType& end);

    NODISCARD String Concat(const String& str);
    NODISCARD String Concat(const char* str);
    NODISCARD String Concat(const char8_t* str);
    NODISCARD String Concat(const ViewType& view);
    template<typename IteratorType>
    NODISCARD String Concat(const IteratorType& begin, const IteratorType& end);

    NODISCARD static String FromUtf16(const char16_t* str);
    NODISCARD static String FromUtf16(const char16_t* str, SizeType length);

    NODISCARD static String FromUtf32(const char32_t* str);
    NODISCARD static String FromUtf32(const char32_t* str, SizeType length);

    NODISCARD static String From(const std::string& str);

    NODISCARD static String From(const std::wstring& str);

    template <typename CharType, typename... Args>
    static String Format(const CharType* fmt, Args&&... args)
    {
        static_assert(std::is_same_v<CharType, char> || std::is_same_v<CharType, char8_t>);
        fmt::basic_memory_buffer<CharType, 250> buffer;
        fmt::detail::vformat_to(buffer, fmt::basic_string_view<CharType>(fmt), fmt::make_format_args<fmt::buffer_context<CharType>>(args...));
        return {buffer.data(), static_cast<SizeType>(buffer.size())};
    }
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

inline char8_t& String::operator[] (SizeType index)
{
    ASSERT(IsValidIndex(index));
    return GetVal().GetPtr()[index];
}

inline char8_t String::operator[] (SizeType index) const
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

inline String::Iterator String::begin() { return Iterator(Data()); }

inline String::ConstIterator String::begin() const{ return ConstIterator(Data()); }

inline String::Iterator String::end() { return Iterator(Data() + Length()); }

inline String::ConstIterator String::end() const { return ConstIterator(Data() + Length()); }

inline String::ReverseIterator String::rbegin() { return ReverseIterator(end()); }

inline String::ConstReverseIterator String::rbegin() const{ return ConstReverseIterator(end()); }

inline String::ReverseIterator String::rend() { return ReverseIterator(begin()); }

inline String::ConstReverseIterator String::rend() const { return ConstReverseIterator(begin()); }

inline String::ConstIterator String::cbegin() const { return begin(); }

inline String::ConstIterator String::cend() const { return end(); }

inline String::ConstReverseIterator String::crbegin() const{ return ConstReverseIterator(end()); }

inline String::ConstReverseIterator String::crend() const { return ConstReverseIterator(begin()); }

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
    return {str.data(), static_cast<SizeType>(str.length())};
}

inline String String::From(const std::wstring& str)
{
    if constexpr (sizeof(std::wstring::value_type) == sizeof(char16_t))
    {
        return String::FromUtf16(reinterpret_cast<const char16_t*>(str.data()), static_cast<SizeType>(str.length()));
    }
    else if constexpr (sizeof(std::wstring::value_type) == sizeof(char32_t))
    {
        return String::FromUtf32(reinterpret_cast<const char32_t*>(str.data()), static_cast<SizeType>(str.length()));
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
    return Append(str, str + CharTraits::length(str));
}

inline String& String::Append(const ViewType& view)
{
    return Append(view.begin(), view.end());
}

template<typename IteratorType>
String& String::Append(const IteratorType& begin, const IteratorType& end)
{
    SizeType length = Length();
    SizeType new_length = length + std::distance(begin, end);
    Reserve(new_length + 1);
    char8_t* p = Data() + length;
    for (auto it = begin; it < end; ++p, ++it)
    {
        CharTraits::assign(*p, *it);
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
    return Prepend(str, str + CharTraits::length(str));
}

inline String& String::Prepend(const ViewType& view)
{
    return Prepend(view.begin(), view.end());
}

template<typename IteratorType>
String& String::Prepend(const IteratorType& begin, const IteratorType& end)
{
    SizeType length = Length();
    SizeType insert_size = std::distance(begin, end);
    SizeType new_length = length + insert_size;

    Reserve(new_length + 1);
    char8_t* p = Data();
    CharTraits::move(p + insert_size, p, length);

    for (auto it = begin; it < end; ++p, ++it)
    {
        CharTraits::assign(*p, *it);
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
    return Concat(str, str + CharTraits::length(str));
}

inline String String::Concat(const ViewType& view)
{
    return Concat(view.begin(), view.end());
}

template<typename IteratorType>
String String::Concat(const IteratorType& begin, const IteratorType& end)
{
    SizeType length = Length();
    SizeType new_length = length + std::distance(begin, end);

    String result;
    result.Reserve(new_length + 1);
    char8_t* p = result.Data();

    CharTraits::copy(p, Data(), length);
    p += length;
    for (auto it = begin; it < end; ++p, ++it)
    {
        CharTraits::assign(*p, *it);
    }
    result.Eos(new_length);

    return result;
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
