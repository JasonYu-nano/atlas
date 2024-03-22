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

class CORE_API StringName
{
public:
    StringName() = default;
    explicit StringName(const String& name);

    NODISCARD uint32 GetNumber() const
    {
        if (number_ != SUFFIX_NUMBER_NONE)
        {
            return SUFFIX_TO_ACTUAL(number_);
        }
        return 0;
    }

    NODISCARD String ToString() const;
    NODISCARD bool IsNone() const
    {
        return name_entry_id_.IsNone();
    }

    bool operator== (const StringName& rhs) const
    {
        return number_ == rhs.number_ && name_entry_id_ == rhs.name_entry_id_;
    }

private:
    template<typename ViewType>
    uint32 SplitNumber(ViewType& view)
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

    static String name_none_;
};

} // namespace atlas
