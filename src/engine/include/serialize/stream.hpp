// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "io/io_types.hpp"
#include "string/string.hpp"
#include "string/string_name.hpp"

namespace atlas
{

/**
 * @brief The abstract write stream used for serialization. Any struct or class can be serialized by implementing `friend void serialize(WriteStream&, const T&)`
 */
class ENGINE_API WriteStream
{
public:
    virtual ~WriteStream() = default;

    /**
     * @brief Returns io buffer.
     * @return
     */
    NODISCARD virtual IOBuffer get_buffer() const = 0;

    template<typename T>
    WriteStream& operator<< (const T& value) { serialize(*this, value); return *this; }

    virtual WriteStream& operator<< (int8 value) { return *this; }
    virtual WriteStream& operator<< (uint8 value) { return *this; }
    virtual WriteStream& operator<< (int16 value) { return *this; }
    virtual WriteStream& operator<< (uint16 value) { return *this; }
    virtual WriteStream& operator<< (int32 value) { return *this; }
    virtual WriteStream& operator<< (uint32 value) { return *this; }
    virtual WriteStream& operator<< (int64 value) { return *this; }
    virtual WriteStream& operator<< (uint64 value) { return *this; }
    virtual WriteStream& operator<< (float value) { return *this; }
    virtual WriteStream& operator<< (double value) { return *this; }
    virtual WriteStream& operator<< (bool value) { return *this; }
    virtual WriteStream& operator<< (const String& value) { return *this; }
    virtual WriteStream& operator<< (StringName value) { return *this; }
};

/**
 * @brief The abstract read stream used for deserialization. Any struct or class can be deserialized by implementing `friend void deserialize(ReadStream&, T&)`
 */
class ENGINE_API ReadStream
{
public:
    virtual ~ReadStream() = default;

    template<typename T>
    ReadStream& operator>> (T& value) { deserialize(*this, value); return *this; }

    virtual ReadStream& operator>> (int8& value) { return *this; }
    virtual ReadStream& operator>> (uint8& value) { return *this; }
    virtual ReadStream& operator>> (int16& value) { return *this; }
    virtual ReadStream& operator>> (uint16& value) { return *this; }
    virtual ReadStream& operator>> (int32& value) { return *this; }
    virtual ReadStream& operator>> (uint32& value) { return *this; }
    virtual ReadStream& operator>> (int64& value) { return *this; }
    virtual ReadStream& operator>> (uint64& value) { return *this; }
    virtual ReadStream& operator>> (float& value) { return *this; }
    virtual ReadStream& operator>> (double& value) { return *this; }
    virtual ReadStream& operator>> (bool& value) { return *this; }
    virtual ReadStream& operator>> (String& value) { return *this; }
    virtual ReadStream& operator>> (StringName& value) { return *this; }
};


}// namespace atlas