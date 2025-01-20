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
 * @brief A template class for fixed size integral.
 * It behaves the same as a normal int type, except that it is not allowed to be compressed by the stream.
 * @tparam T
 */
template<std::integral T>
struct FixedInt
{
    T value = 0;

    FixedInt() = default;
    FixedInt(T value) : value(value) {}
    operator T() const { return value; }
    FixedInt& operator=(const FixedInt& other) = default;
    FixedInt& operator=(T v) { value = v; return *this; }
};

typedef FixedInt<int32>     Fixed32;
typedef FixedInt<uint32>    FixedU32;
typedef FixedInt<int64>     Fixed64;
typedef FixedInt<uint64>    FixedU64;

/**
 * @class WriteStream
 * @brief The abstract write stream used for serialization. Any struct or class can be serialized by implementing `friend void serialize(WriteStream&, const T&)`
 */
class CORE_API WriteStream
{
public:
    virtual ~WriteStream() = default;

    /**
     * @brief Returns the buffer.
     * @return A buffer containing the serialized data.
     */
    NODISCARD virtual IOBuffer get_buffer() const = 0;

    /**
     * @brief Template function to serialize a value into the stream.
     * @tparam T The type of the value to serialize.
     * @param value The value to serialize.
     * @return A reference to the WriteStream.
     */
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
    virtual WriteStream& operator<< (FixedU32 value) { return operator<< (value.value); }
    virtual WriteStream& operator<< (FixedU64 value) { return operator<< (value.value); }
    virtual WriteStream& operator<< (float value) { return *this; }
    virtual WriteStream& operator<< (double value) { return *this; }
    virtual WriteStream& operator<< (bool value) { return *this; }
    virtual WriteStream& operator<< (const String& value) { return *this; }
    virtual WriteStream& operator<< (StringName value) { return *this; }

    /**
     * @brief Get the stream total size.
     * @return The total size of the stream.
     */
    NODISCARD virtual size_t size() const { return 0; }

    /**
     * @brief Get the current position for writing data.
     * @return The current write position.
     */
    NODISCARD virtual size_t tell() const { return 0; }

    /**
     * @brief Set the current position for writing data.
     * This will do nothing if the position is over the stream size.
     * @param position The new write position.
     */
    virtual void seek(size_t position) {}
};

/**
 * @class ReadStream
 * @brief The abstract read stream used for deserialization. Any struct or class can be deserialized by implementing `friend void deserialize(ReadStream&, T&)`
 */
class CORE_API ReadStream
{
public:
    virtual ~ReadStream() = default;

    /**
     * @brief Template function to deserialize a value from the stream.
     * @tparam T The type of the value to deserialize.
     * @param value The value to deserialize.
     * @return A reference to the ReadStream.
     */
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
    virtual ReadStream& operator>> (FixedU32& value) { return operator>> (value.value); }
    virtual ReadStream& operator>> (FixedU64& value) { return operator>> (value.value); }
    virtual ReadStream& operator>> (float& value) { return *this; }
    virtual ReadStream& operator>> (double& value) { return *this; }
    virtual ReadStream& operator>> (bool& value) { return *this; }
    virtual ReadStream& operator>> (String& value) { return *this; }
    virtual ReadStream& operator>> (StringName& value) { return *this; }

    /**
     * @brief Returns whether the end of the stream was reached.
     * @return True if the end of the stream is reached, false otherwise.
     */
    virtual bool eof() = 0;

    /**
     * @brief Get the stream total size.
     * @return The total size of the stream.
     */
    NODISCARD virtual size_t size() { return 0; }

    /**
     * @brief Get the current position for reading data.
     * @return The current read position.
     */
    NODISCARD virtual size_t tell() { return INDEX_NONE; }

    /**
     * @brief Set the current position for reading data.
     * This will do nothing if the position is over the stream size.
     * @param position The new read position.
     */
    virtual void seek(size_t position) {}
};

/**
 * @class ScopeStreamSeek
 * @brief Helper class to seek and restore the position of a stream.
 * @tparam T Any stream class.
 */
template<typename T> requires(std::is_base_of_v<WriteStream, T> || std::is_base_of_v<ReadStream, T>)
class ScopeStreamSeek
{
public:
    ScopeStreamSeek(T& stream, size_t new_pos) : stream_(stream)
    {
        old_pos = stream.tell();
        stream_.seek(new_pos);
    }

    ~ScopeStreamSeek() { stream_.seek(old_pos); }

private:
    T& stream_;
    size_t old_pos;
};


}// namespace atlas