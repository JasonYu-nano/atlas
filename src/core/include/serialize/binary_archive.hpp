// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "stream.hpp"

namespace atlas
{

/**
 * @class BinaryArchiveWriter
 * @brief A class provides functionality to serialize various data types into a binary format.
 */
class CORE_API BinaryArchiveWriter : public WriteStream
{
public:
    ~BinaryArchiveWriter() override = default;

    template<typename T>
    WriteStream& operator<< (const T& value) { serialize(*this, value); return *this; }

    WriteStream& operator<< (int8 value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (uint8 value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (int16 value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (uint16 value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (int32 value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (uint32 value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (int64 value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (uint64 value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (float value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (double value) override
    {
        serialize_numeric(value);
        return *this;
    }

    WriteStream& operator<< (bool value) override
    {
        int8 i = value ? 1 : 0;
        serialize_numeric(i);
        return *this;
    }

    WriteStream& operator<< (const String& value) override
    {
        const size_t len = value.length();
        operator<<(len);
        write_bytes(reinterpret_cast<const byte*>(value.data()), len);
        return *this;
    }

    WriteStream& operator<< (StringName value) override
    {
        operator<<(value.to_string());
        return *this;
    }

    /**
     * @brief Get the buffer containing the serialized binary data.
     * @return An buffer containing the binary data.
     */
    NODISCARD IOBuffer get_buffer() const override
    {
        return buffer_;
    }

    /**
     * @brief Get the size of the binary stream.
     * @return The size of the binary stream.
     */
    NODISCARD size_t size() const override
    {
        return buffer_.size();
    }

    /**
     * @brief Get the current write position in the binary stream.
     * @return The current write position.
     */
    NODISCARD size_t tell() const override
    {
        return write_position_;
    }

    /**
     * @brief Set the write position in the binary stream.
     * @param position The new write position.
     */
    void seek(size_t position) override
    {
        if (write_position_ <= size())
        {
            write_position_ = position;
        }
    }

protected:
    /**
     * @brief Serialize a numeric value into the binary stream.
     * @tparam T The type of the numeric value to serialize.
     * @param value The value to serialize.
     */
    template<typename T>
    void serialize_numeric(T value) requires(std::is_arithmetic_v<T>)
    {
        write_bytes(reinterpret_cast<byte*>(&value), sizeof(T));
    }

    /**
     * @brief Write a sequence of bytes to the binary stream.
     * @param bytes The pointer to the bytes to write.
     * @param byte_size The number of bytes to write.
     */
    void write_bytes(const byte* bytes, size_t byte_size);

    IOBuffer buffer_;
    size_t write_position_ = 0;
};

/**
 * @class BinaryArchiveReader
 * @brief A class for reading data from a binary stream.
 */
class CORE_API BinaryArchiveReader : public ReadStream
{
public:
    explicit BinaryArchiveReader(const IOBuffer& buffer) : buffer_(buffer) {}

    ~BinaryArchiveReader() override = default;

    template<typename T>
    ReadStream& operator>> (T& value) { deserialize(*this, value); return *this; }

    ReadStream& operator>> (int8& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (uint8& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (int16& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (uint16& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (int32& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (uint32& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (int64& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (uint64& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (float& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (double& value) override
    {
        deserialize_numeric(value);
        return *this;
    }

    ReadStream& operator>> (bool& value) override
    {
        int8 i = 0;
        deserialize_numeric(i);
        value = i > 0;
        return *this;
    }

    ReadStream& operator>> (String& value) override
    {
        size_t len;
        operator>>(len);
        const byte* start = buffer_.data() + read_position_;
        read_position_ += len;
        value = String(reinterpret_cast<String::const_pointer>(start), len);
        return *this;
    }

    ReadStream& operator>> (StringName& value) override
    {
        String str;
        operator>>(str);
        value = str;
        return *this;
    }

    /**
     * @brief Check if the end of the binary stream has been reached.
     * @return True if the end of the stream is reached, false otherwise.
     */
    bool eof() override
    {
        return read_position_ >= buffer_.size();
    }

    /**
     * @brief Get the current read position in the binary stream.
     * @return The current read position.
     */
    size_t tell() override
    {
        return read_position_;
    }

    /**
     * @brief Set the read position in the binary stream.
     * @param position The new read position.
     */
    void seek(size_t position) override
    {
        if (position < buffer_.size())
        {
            read_position_ = position;
        }
    }
protected:
    /**
     * @brief Deserialize a numeric value from the binary stream.
     * @tparam T The type of the numeric value to deserialize.
     * @param value The value to deserialize.
     */
    template<typename T>
    void deserialize_numeric(T& value) requires(std::is_arithmetic_v<T>)
    {
        byte* begin = &buffer_[read_position_];
        read_position_ += sizeof(T);
        value = *reinterpret_cast<T*>(begin);
    }

    size_t read_position_{ 0 };
    IOBuffer buffer_;
};

}// namespace atlas
