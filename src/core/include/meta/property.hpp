// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "meta/meta_types.hpp"
#include "utility/stream.hpp"

namespace atlas
{

class CORE_API Property : public MetaType
{
public:
    virtual ~Property() = default;

    void serialize(WriteStream& stream, void* data) const
    {
        if (has_flag(EMetaFlag::Serializable))
        {
            serialize_impl(stream, data);
        }
    }

    virtual void deserialize(ReadStream& stream, void* data)
    {
        if (has_flag(EMetaFlag::Serializable))
        {

        }
    }

protected:
    virtual void serialize_impl(WriteStream& stream, void* data) const = 0;
};

}// namespace atlas
