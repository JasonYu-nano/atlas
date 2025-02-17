// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "binary_archive.hpp"

namespace atlas
{

/**
 * @class CompactBinaryArchiveWriter
 * @brief A class provides functionality to serialize various data types into a compact binary format.
 * If you want to serialize fixed length integers, use FixedInt instead of int, uint, etc.
 */
class CORE_API CompactBinaryArchiveWriter : public BinaryArchiveWriter
{
public:
    ~CompactBinaryArchiveWriter() override = default;

    WriteStream& operator<< (int8 value) override
    {
        serialize_varint(math::zigzag_encode8(value));
        return *this;
    }

    WriteStream& operator<< (uint8 value) override
    {
        serialize_varint(value);
        return *this;
    }

    WriteStream& operator<< (int16 value) override
    {
        serialize_varint(math::zigzag_encode16(value));
        return *this;
    }

    WriteStream& operator<< (uint16 value) override
    {
        serialize_varint(value);
        return *this;
    }

    WriteStream& operator<< (int32 value) override
    {
        serialize_varint(math::zigzag_encode32(value));
        return *this;
    }

    WriteStream& operator<< (uint32 value) override
    {
        serialize_varint(value);
        return *this;
    }

    WriteStream& operator<< (int64 value) override
    {
        serialize_varint(math::zigzag_encode64(value));
        return *this;
    }

    WriteStream& operator<< (uint64 value) override
    {
        serialize_varint(value);
        return *this;
    }

    WriteStream& operator<< (FixedU32 value) override
    {
        serialize_numeric(value.value);
        return *this;
    }

    WriteStream& operator<< (FixedU64 value) override
    {
        serialize_numeric(value.value);
        return *this;
    }

protected:
    /**
     * @brief Serialize a variable-length integer into the binary stream.
     * @tparam T The type of the integer to serialize.
     * @param value The integer value to serialize.
     */
    template<std::integral T>
    void serialize_varint(T value)
    {
        static const int mask = 128;

        while (value >= mask)
        {
            byte b = static_cast<byte>((value & (mask-1)) | mask);
            write_bytes(&b, 1);
            value >>= 7;
        }
        byte b = static_cast<byte>(value);
        write_bytes(&b, 1);
    }
};

/**
 * @class CompactBinaryArchiveReader
 * @brief A class for reading compact binary data from a stream.
 */
class CORE_API CompactBinaryArchiveReader : public BinaryArchiveReader
{
public:
    explicit CompactBinaryArchiveReader(const IOBuffer& buffer) : BinaryArchiveReader(buffer) {}

    ~CompactBinaryArchiveReader() override = default;

    ReadStream& operator>> (int8& value) override
    {
        uint8 n;
        deserialize_varint(n);
        value = math::zigzag_decode8(n);
        return *this;
    }

    ReadStream& operator>> (uint8& value) override
    {
        deserialize_varint(value);
        return *this;
    }

    ReadStream& operator>> (int16& value) override
    {
        uint16 n;
        deserialize_varint(n);
        value = math::zigzag_decode16(n);
        return *this;
    }

    ReadStream& operator>> (uint16& value) override
    {
        deserialize_varint(value);
        return *this;
    }

    ReadStream& operator>> (int32& value) override
    {
        uint32 n;
        deserialize_varint(n);
        value = math::zigzag_decode32(n);
        return *this;
    }

    ReadStream& operator>> (uint32& value) override
    {
        deserialize_varint(value);
        return *this;
    }

    ReadStream& operator>> (int64& value) override
    {
        uint64 n;
        deserialize_varint(n);
        value = math::zigzag_decode64(n);
        return *this;
    }

    ReadStream& operator>> (uint64& value) override
    {
        deserialize_varint(value);
        return *this;
    }

    ReadStream& operator>> (FixedU32& value) override
    {
        deserialize_numeric(value.value);
        return *this;
    }

    ReadStream& operator>> (FixedU64& value) override
    {
        deserialize_numeric(value.value);
        return *this;
    }
protected:
    /**
     * @brief Deserialize a variable-length integer from the binary stream.
     * @tparam T The type of the integer to deserialize.
     * @param value The integer value to deserialize.
     */
    template<std::integral T>
    void deserialize_varint(T& value)
    {
        uint64 result = 0;
        for (uint32 shift = 0; shift <= 63 && read_position_ < buffer_.size(); shift += 7)
        {
            uint64 byte = buffer_[read_position_];
            read_position_++;
            if (byte & 128)
            {
                // More bytes are present
                result |= ((byte & 127) << shift);
            }
            else
            {
                result |= (byte << shift);
                value = static_cast<T>(result);
                return;
            }
        }
    }
};

}// namespace atlas
