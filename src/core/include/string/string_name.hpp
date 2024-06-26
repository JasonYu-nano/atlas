// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "string/string_name_pool.hpp"
#include "string/string.hpp"

namespace atlas
{

#define SUFFIX_NUMBER_NONE (0)
#define ACTUAL_TO_SUFFIX(number) ((number) + 1)
#define SUFFIX_TO_ACTUAL(number) ((number) - 1)

/**
 * StringNames are stored as a combination of an index into a table of unique strings and an instance number.
 * Names are case-insensitive, but case-preserving (when NAME_PRESERVING_CASE_SENSITIVE is 1)
 */
class CORE_API StringName
{
public:
    StringName() noexcept = default;
    explicit StringName(const String& name)
    {
        StringView view(name);
        construct(view);
    }
    StringName(const char* name)
    {
        StringView view(name);
        construct(view);
    }

    StringName(StringView name)
    {
        construct(name);
    }

    StringName(const StringName& rhs) noexcept = default;
    StringName(StringName&& rhs) noexcept = default;
    StringName& operator= (const StringName& rhs) noexcept = default;
    StringName& operator= (StringView rhs)
    {
        construct(rhs);
        return *this;
    };

    NODISCARD uint32 compress_id() const
    {
        return name_entry_id_.compress_id();
    }
    /**
     * @brief Get suffix number of name.
     * @return
     */
    NODISCARD uint32 suffix_number() const
    {
        if (number_ != SUFFIX_NUMBER_NONE)
        {
            return SUFFIX_TO_ACTUAL(number_);
        }
        return 0;
    }
    /**
     * @brief Convert StringName to String.
     * @return
     */
    NODISCARD String to_string() const
    {
        if (name_entry_id_.is_none())
        {
            return name_none_;
        }

        if (number_ == SUFFIX_NUMBER_NONE)
        {
            return details::NameEntryPool::get().get_entry(name_entry_id_);
        }

        String prefix_name = details::NameEntryPool::get().get_entry(name_entry_id_);
        return String::format("{0}_{1}", prefix_name, SUFFIX_TO_ACTUAL(number_));
    }
    /**
     * @brief Convert StringName to String, excludes number part.
     * @return
     */
    NODISCARD String to_lexical() const
    {
        if (name_entry_id_.is_none())
        {
            return name_none_;
        }

        return details::NameEntryPool::get().get_entry(name_entry_id_);
    }
    /**
     * @brief Judges whether StringName is illegal or not.
     * @return
     */
    NODISCARD bool is_none() const
    {
        return name_entry_id_.is_none();
    }
    /**
     * @brief Compares two names by id.
     * @param rhs
     * @return
     */
    NODISCARD int32 compare(const StringName& rhs) const
    {
        int32 diff = name_entry_id_.compare(rhs.name_entry_id_);
        if (diff != 0)
        {
            diff = static_cast<int32>(number_ - rhs.number_);
        }
        return diff;
    }
    /**
     * @brief Compares two names by alphabetical order. Slow but stable.
     * @param rhs
     * @return
     */
    NODISCARD int32 compare_lexical(const StringName& rhs) const
    {
        if (name_entry_id_ == rhs.name_entry_id_)
        {
            return static_cast<int32>(number_ - rhs.number_);
        }
        auto my_view = details::NameEntryPool::get().get_entry_view(name_entry_id_);
        auto rhs_view = details::NameEntryPool::get().get_entry_view(rhs.name_entry_id_);
#if NAME_PRESERVING_CASE_SENSITIVE
        return my_view.compare_insensitive(rhs_view);
#else
        return my_view.compare(rhs_view);
#endif
    }

    bool operator== (const StringName& rhs) const
    {
        return name_entry_id_ == rhs.name_entry_id_ && number_ == rhs.number_;
    }
    bool operator!= (const StringName& rhs) const
    {
        return name_entry_id_ != rhs.name_entry_id_ || number_ != rhs.number_;
    }
    bool operator< (const StringName& rhs) const
    {
        return compare(rhs) < 0;
    }
    bool operator> (const StringName& rhs) const
    {
        return compare(rhs) > 0;
    }

private:
    template<typename ViewType>
    void construct(ViewType& view)
    {
        number_ = split_number(view);
        if (!view.empty())
        {
            name_entry_id_ = details::NameEntryPool::get().get_entry_id(view);
        }
        else
        {
            number_ = SUFFIX_NUMBER_NONE;
        }
    }

    template<typename ViewType>
    uint32 split_number(ViewType& view)
    {
        using const_pointer = ViewType::const_pointer;

        const_pointer str = view.data();
        size_t len = view.length();

        size_t number_count = 0;
        for (const_pointer p_char = str + len - 1; p_char >= str && int32(*p_char) >= '0' && int32(*p_char) <= '9'; --p_char )
        {
            ++number_count;
        }

        // number suffix must start with _ and skip number like 01
        const_pointer first_number = str + len - number_count;
        if (number_count > 0 && number_count < len && int32(*(first_number - 1)) == '_')
        {
            if (number_count == 1 || int32(*first_number) != '0')
            {
                const char* unsafe_first = reinterpret_cast<const char*>(first_number);
                int64 number = std::strtoll(unsafe_first, nullptr, 10);
                if (number < std::numeric_limits<int32>::max())
                {
                    view.remove_suffix(number_count + 1);
                    return static_cast<uint32>(ACTUAL_TO_SUFFIX(number));
                }
            }
        }

        return SUFFIX_NUMBER_NONE;
    }

    NameEntryID name_entry_id_;
    uint32 number_{ SUFFIX_NUMBER_NONE };

    static inline String name_none_ = String("none");
};

} // namespace atlas

template<>
struct std::hash<atlas::StringName>
{
    NODISCARD size_t operator()(const atlas::StringName& name) const noexcept
    {
        size_t hash = (size_t(name.compress_id()) << 32) + name.suffix_number();
        return hash;
    }
};
