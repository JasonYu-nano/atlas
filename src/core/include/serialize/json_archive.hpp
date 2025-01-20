// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "stream.hpp"
#include "utility/json.hpp"

namespace atlas
{

/**
 * @class JsonArchiveWriter
 * @brief A class provides functionality to serialize various data types into a JSON array.
 */
class CORE_API JsonArchiveWriter : public WriteStream
{
public:
    JsonArchiveWriter() :json_(EJsonValueType::array) {}

    template<typename T>
    WriteStream& operator<< (const T& value) { serialize(*this, value); return *this; }

    WriteStream& operator<< (int8 value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (uint8 value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (int16 value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (uint16 value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (int32 value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (uint32 value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (int64 value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (uint64 value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (float value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (double value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (bool value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (const String& value) override
    {
        write_value(value);
        return *this;
    }

    WriteStream& operator<< (StringName value) override
    {
        return operator<<(value.to_string());
    }

    /**
     * @brief Get the buffer containing the serialized JSON data.
     * @return An IOBuffer containing the JSON data.
     */
    NODISCARD IOBuffer get_buffer() const override
    {
        return IOBuffer(json_.dump());
    }

    /**
     * @brief Get the JSON object.
     * @return A constant reference to the JSON object.
     */
    NODISCARD const Json& get_json() const
    {
        return json_;
    }

    /**
     * @brief Get the size of the JSON array.
     * @return The size of the JSON array.
     */
    NODISCARD size_t size() const override
    {
        return json_.size();
    }

    /**
     * @brief Get the current write position in the JSON array.
     * @return The current write position.
     */
    NODISCARD size_t tell() const override
    {
        return write_position_;
    }

    /**
     * @brief Set the write position in the JSON array.
     * @param position The new write position.
     */
    void seek(size_t position) override
    {
        if (write_position_ <= size())
        {
            write_position_ = position;
        }
    }

private:
    template<typename T>
    void write_value(T v)
    {
        size_t size = json_.size();
        if (write_position_ == size)
        {
            json_.push_back(Json(std::forward<T>(v)));
            ++write_position_;
        }
        else if (write_position_ < size)
        {
            auto&& old_value = json_.at(write_position_);
            auto it = json_.begin() + write_position_;
            it = json_.erase(it);
            json_.insert(it, Json(std::forward<T>(v)));
        }
        else
        {
            std::unreachable();
        }
    }

    Json json_;
    size_t write_position_ = 0;
};

/**
 * @class JsonArchiveReader
 * @brief A class for reading data from a JSON stream.
 */
class CORE_API JsonArchiveReader : public ReadStream
{
public:
    explicit JsonArchiveReader(const Json& json) :json_(json) {}

    template<typename T>
    ReadStream& operator>> (T& value) { deserialize(*this, value); return *this; }

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

    ReadStream& operator>> (bool& value) override
    {
        if (json_.is_array() && read_position_ < json_.size())
        {
            Json& jobject = json_.at(read_position_);
            if (jobject.is_boolean())
            {
                value = jobject.get<bool>();
            }
        }
        ++read_position_;
        return *this;
    }

    ReadStream& operator>> (String& value) override
    {
        if (json_.is_array() && read_position_ < json_.size())
        {
            Json& jobject = json_.at(read_position_);
            if (jobject.is_string())
            {
                value = jobject.get<std::string>().data();
            }
        }
        ++read_position_;
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
     * @brief Check if the end of the JSON array has been reached.
     * @return True if the end of the array is reached, false otherwise.
     */
    bool eof() override
    {
        return !(json_.is_array() && read_position_ < json_.size());
    }

    /**
     * @brief Get the current read position in the JSON array.
     * @return The current read position.
     */
    NODISCARD size_t tell() override
    {
        return read_position_;
    }

    /**
     * @brief Set the read position in the JSON array.
     * @param position The new read position.
     */
    void seek(size_t position) override
    {
        if (position < json_.size())
        {
            read_position_ = position;
        }
    }

private:
    template<typename T> requires (std::is_integral_v<T>)
    void deserialize_integral(T& value)
    {
        if (json_.is_array() && read_position_ < json_.size())
        {
            Json& jobject = json_.at(read_position_);
            if (jobject.is_number_integer())
            {
                if constexpr (std::is_unsigned_v<T>)
                {
                    value = math::cast_checked<T>(jobject.get<uint64>());
                }
                else
                {
                    value = math::cast_checked<T>(jobject.get<int64>());
                }
            }
        }
        ++read_position_;
    }

    template<typename T> requires (std::is_floating_point_v<T>)
    void deserialize_float_point(T& value)
    {
        if (json_.is_array() && read_position_ < json_.size())
        {
            Json& jobject = json_.at(read_position_);
            if (jobject.is_number_float())
            {
                value = static_cast<T>(jobject.get<double>());
            }
        }
        ++read_position_;
    }

    size_t read_position_{ 0 };
    Json json_;
};

}// namespace atlas