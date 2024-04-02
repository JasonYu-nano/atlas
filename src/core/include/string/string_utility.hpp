// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>

#include "core_def.hpp"
#include "math/atlas_math.hpp"

namespace atlas
{
/**
 * @brief Converts characters to lower. Only converts ASCII characters.
 * @tparam CharType
 * @tparam CharTraits
 * @param ch
 * @return
 */
template<typename CharType, typename CharTraits = std::char_traits<CharType>>
constexpr CharType ToLower(CharType ch)
{
    return CharTraits::to_char_type(CharTraits::to_int_type(ch) + ((CharTraits::to_int_type(ch) - 'A' < 26u) << 5));
}
/**
 * @brief Converts characters to upper. Only converts ASCII characters.
 * @tparam CharType
 * @tparam CharTraits
 * @param ch
 * @return
 */
template<typename CharType, typename CharTraits = std::char_traits<CharType>>
constexpr CharType ToUpper(CharType ch)
{
    return CharTraits::to_char_type(CharTraits::to_int_type(ch) - ((CharTraits::to_int_type(ch) - 'a' < 26u) << 5));
}

template<typename CharType>
class StringView : public std::basic_string_view<CharType>
{
    using base = std::basic_string_view<CharType>;
public:
    constexpr StringView() noexcept : base() {}

    constexpr StringView(const StringView& view) noexcept : base(view) {}

    constexpr StringView(const typename base::const_pointer str) noexcept : base(str) {}

    StringView(nullptr_t) = delete;

    constexpr StringView(const typename base::const_pointer str, const typename base::size_type count) noexcept : base(str, count) {}

    template <std::contiguous_iterator Iter, std::sized_sentinel_for<Iter> Sent>
    constexpr StringView(Iter first, Sent last) noexcept : base(first, last) {}

    template <class RangeType>
    constexpr explicit StringView(RangeType&& range) noexcept : base(std::forward<RangeType>(range)) {}

    constexpr int32 compare_insensitive(const StringView rhs) const
    {
        auto first = this->data();
        auto second = rhs.data();

        auto first_length = this->length();
        auto second_length = rhs.length();

        typename base::size_type count = math::Min(first_length, second_length);
        for (; 0 < count; --count, ++first, ++second)
        {
            auto lower_first = ToLower(*first);
            auto lower_second = ToLower(*second);

            if (lower_first != lower_second)
            {
                return lower_first < lower_second ? -1 : 1;
            }
        }

        return first_length < second_length ? -1 : first_length == second_length ? 0 : 1;
    }
};

} // namespace atlas