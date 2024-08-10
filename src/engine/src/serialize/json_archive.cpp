// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "serialize/json_archive.hpp"

namespace atlas
{

ReadStream& JsonArchiveReader::operator>>(bool& value)
{
    if (json_.is_array() && cursor_ < json_.size())
    {
        Json& jobject = json_.at(cursor_);
        if (jobject.is_boolean())
        {
            value = jobject.get<bool>();
        }
    }
    ++cursor_;
    return *this;
}

ReadStream& JsonArchiveReader::operator>>(String& value)
{
    if (json_.is_array() && cursor_ < json_.size())
    {
        Json& jobject = json_.at(cursor_);
        if (jobject.is_string())
        {
            value = jobject.get<std::string>().data();
        }
    }
    ++cursor_;
    return *this;
}

ReadStream& JsonArchiveReader::operator>>(StringName& value)
{
    String str;
    operator>>(str);
    value = str;
    return *this;
}

}// namespace atlas