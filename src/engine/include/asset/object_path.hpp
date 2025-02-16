// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "string/string.hpp"

namespace atlas
{

/**
 * @class ObjectPath
 * @brief Represents a relative path to an object in a package.
 * eg: engine:dir/package.object
 */
class ENGINE_API ObjectPath
{
public:
    using value_type        = String::value_type;
    using pointer           = String::pointer;
    using const_pointer     = String::const_pointer;

    ObjectPath() = default;
    ObjectPath(StringView path) : text_(path) {}

    bool operator==(const ObjectPath& path) const = default;

    NODISCARD StringView get_owner_assembly() const
    {
        const auto first = text_.data();
        const auto last  = first + text_.size();
        auto first_name = find_first_name(first, last);
        if (first_name != last)
        {
            return StringView(first, first_name - first);
        }
        return {};
    }

    NODISCARD StringView get_package_prefix() const
    {
        const auto first = text_.data();
        const auto last  = first + text_.size();
        auto first_name = find_first_name(first, last);
        auto last_name = find_last_name(first, last);

        if (first_name == last) // assembly name not found.
        {
            return StringView(first, last_name - first);
        }

        if (++first_name < last_name)
        {
            return StringView(first_name, last_name - first_name);
        }
        return {};
    }

    NODISCARD StringView get_object_name() const
    {
        const auto first = text_.data();
        const auto last  = first + text_.size();
        auto last_name = find_last_name(first, last);
        if (last_name < last)
        {
            return StringView(++last_name, last - last_name - 1);
        }
        return {};
    }

private:
    const_pointer find_first_name(const_pointer first, const_pointer last) const
    {
        pointer mutable_first = const_cast<pointer>(first);
        while (mutable_first != last && *mutable_first != ':')
        {
            ++mutable_first;
        }
        return mutable_first;
    }

    const_pointer find_last_name(const_pointer first, const_pointer last) const
    {
        pointer mutable_last = const_cast<pointer>(last);
        while (first != mutable_last && *mutable_last != '.')
        {
            --mutable_last;
        }
        return mutable_last;
    }

    String text_;
};

}// namespace atlas
