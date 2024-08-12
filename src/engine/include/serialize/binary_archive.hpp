#pragma once

#include "stream.hpp"

namespace atlas
{

class ENGINE_API BinaryArchiveWriter : public WriteStream
{
public:
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
        serialize_numeric(value.length());
        buffer_.append(value);
        return *this;
    }

    WriteStream& operator<< (StringName value) override
    {
        operator<<(value.to_string());
        return *this;
    }

    NODISCARD IOBuffer get_buffer() const override
    {
        return buffer_;
    }

private:
    template<typename T>
    void serialize_numeric(T value) requires(std::is_arithmetic_v<T>)
    {
        byte bytes[sizeof(T)];
        std::memcpy(bytes, reinterpret_cast<byte*>(&value), sizeof(T));
        buffer_.append(bytes);
    }

    IOBuffer buffer_;
};

class ENGINE_API BinaryArchiveReader : public ReadStream
{
public:
    explicit BinaryArchiveReader(const IOBuffer& buffer) : buffer_(buffer) {}

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
        deserialize_numeric(len);
        const byte* start = buffer_.data() + cursor_;
        cursor_ += len;
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
private:
    template<typename T>
    void deserialize_numeric(T& value) requires(std::is_arithmetic_v<T>)
    {
        byte* begin = &buffer_[cursor_];
        cursor_ += sizeof(T);
        value = *reinterpret_cast<T*>(begin);
    }

    size_t cursor_{ 0 };
    IOBuffer buffer_;
};

}// namespace atlas
