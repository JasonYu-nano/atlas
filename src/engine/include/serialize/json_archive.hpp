// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "io/io_types.hpp"
#include "stream.hpp"
#include "utility/json.hpp"

namespace atlas
{

class ENGINE_API JsonArchiveWriter : public WriteStream
{
public:
    JsonArchiveWriter() :json_(EJsonValueType::array) {}

    WriteStream& operator<< (int8 value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (uint8 value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (int16 value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (uint16 value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (int32 value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (uint32 value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (int64 value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (uint64 value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (float value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (double value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (bool value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (const String& value) override
    {
        json_.push_back(Json(value));
        return *this;
    }

    WriteStream& operator<< (StringName value) override
    {
        return operator<<(value.to_string());
    }

    NODISCARD IOBuffer get_stream_buffer() const
    {
        return IOBuffer(json_.dump());
    }

private:
    Json json_;
};

class ENGINE_API JsonArchiveReader : public ReadStream
{
public:
    explicit JsonArchiveReader(const Json& json) :json_(json) {}

    ReadStream& operator>> (int8& value) override
    {
        deserialize_integral(value);
        return *this;
    }

    ReadStream& operator>> (uint8& value) override
    {
        deserialize_integral(value);
        return *this;
    }

    ReadStream& operator>> (int16& value) override
    {
        deserialize_integral(value);
        return *this;
    }

    ReadStream& operator>> (uint16& value) override
    {
        deserialize_integral(value);
        return *this;
    }

    ReadStream& operator>> (int32& value) override
    {
        deserialize_integral(value);
        return *this;
    }

    ReadStream& operator>> (uint32& value) override
    {
        deserialize_integral(value);
        return *this;
    }

    ReadStream& operator>> (int64& value) override
    {
        deserialize_integral(value);
        return *this;
    }

    ReadStream& operator>> (uint64& value) override
    {
        deserialize_integral(value);
        return *this;
    }

    ReadStream& operator>> (float& value) override
    {
        deserialize_float_point(value);
        return *this;
    }

    ReadStream& operator>> (double& value) override
    {
        deserialize_float_point(value);
        return *this;
    }

    ReadStream& operator>> (bool& value) override;

    ReadStream& operator>> (String& value) override;

    ReadStream& operator>> (StringName& value) override;

private:
    template<typename T> requires (std::is_integral_v<T>)
    void deserialize_integral(T& value)
    {
        if (json_.is_array() && cursor_ < json_.size())
        {
            Json& jobject = json_.at(cursor_);
            if (jobject.is_number_integer())
            {
                value = math::cast_checked<T>(jobject.get<int64>());
            }
        }
        ++cursor_;
    }

    template<typename T> requires (std::is_floating_point_v<T>)
    void deserialize_float_point(T& value)
    {
        if (json_.is_array() && cursor_ < json_.size())
        {
            Json& jobject = json_.at(cursor_);
            if (jobject.is_number_float())
            {
                value = static_cast<T>(jobject.get<double>());
            }
        }
        ++cursor_;
    }

    size_t cursor_{ 0 };
    Json json_;
};

}// namespace atlas