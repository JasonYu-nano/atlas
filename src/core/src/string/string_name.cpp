// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "string/string_name.hpp"

namespace atlas
{

String StringName::name_none_("none");

StringName::StringName(const String& name)
{
    String::view_type view(name);
    number_ = SplitNumber(view);
    if (!view.empty())
    {
        name_entry_id_ = details::NameEntryPool::Get().GetEntryID(view);
    }
    else
    {
        number_ = SUFFIX_NUMBER_NONE;
    }
}

String StringName::ToString() const
{
    if (name_entry_id_.IsNone())
    {
        return name_none_;
    }

    if (number_ == SUFFIX_NUMBER_NONE)
    {
        return details::NameEntryPool::Get().GetEntry(name_entry_id_);
    }

    String prefix_name = details::NameEntryPool::Get().GetEntry(name_entry_id_);
    return String::Format("{0}_{1}", prefix_name, SUFFIX_TO_ACTUAL(number_));
}

} // namespace atlas