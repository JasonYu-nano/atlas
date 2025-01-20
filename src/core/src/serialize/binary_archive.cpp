// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "serialize/binary_archive.hpp"

namespace atlas
{

void BinaryArchiveWriter::write_bytes(const byte* bytes, size_t byte_size)
{
    const size_t size = buffer_.size();
    if (write_position_ == size)
    {
        buffer_.append(std::span(bytes, byte_size));
        write_position_ += byte_size;
    }
    else if (write_position_ < size)
    {
        if (write_position_ + byte_size > size)
        {
            buffer_.resize(write_position_ + byte_size);
        }
        std::memmove(buffer_.data() + write_position_, bytes, byte_size);
        write_position_ += byte_size;
    }
    else
    {
        std::unreachable();
    }
}

}// namespace atlas