#pragma once

#include "utility/stream.hpp"

namespace atlas::test
{

class PrintWriter : public WriteStream
{
public:
    PrintWriter() = default;

    template<typename T>
    WriteStream& operator<< (const T& value) { serialize(*this, value); return *this; }

    WriteStream& operator<< (int8 value) override
    {
        printf("%d\n", value);
        return *this;
    }

    WriteStream& operator<< (uint8 value) override
    {
        printf("%d\n", value);
        return *this;
    }

    WriteStream& operator<< (int16 value) override
    {
        printf("%d\n", value);
        return *this;
    }

    WriteStream& operator<< (uint16 value) override
    {
        printf("%d\n", value);
        return *this;
    }

    WriteStream& operator<< (int32 value) override
    {
        printf("%d\n", value);
        return *this;
    }

    WriteStream& operator<< (uint32 value) override
    {
        printf("%d\n", value);
        return *this;
    }

    WriteStream& operator<< (int64 value) override
    {
        printf("%lld\n", value);
        return *this;
    }

    WriteStream& operator<< (uint64 value) override
    {
        printf("%lld\n", value);
        return *this;
    }

    WriteStream& operator<< (float value) override
    {
        printf("%f\n", value);
        return *this;
    }

    WriteStream& operator<< (double value) override
    {
        printf("%f\n", value);
        return *this;
    }

    WriteStream& operator<< (bool value) override
    {
        printf("%d\n", value);
        return *this;
    }

    WriteStream& operator<< (const String& value) override
    {
        printf("%s\n", value.data());
        return *this;
    }

    WriteStream& operator<< (StringName value) override
    {
        return operator<<(value.to_string());
    }

    NODISCARD IOBuffer get_buffer() const override
    {
        return IOBuffer();
    }
};

}//namespace atlas::test
