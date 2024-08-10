// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "io_types.hpp"
#include "async/task.hpp"
#include "file_system/path.hpp"

namespace atlas
{

class IIOBackend
{
public:
    virtual ~IIOBackend() = default;

    virtual Task<size_t> async_read(Path file, IOBuffer& buffer, size_t read_size, size_t offset, EIOPriority priority) = 0;

    virtual Task<size_t> async_write(Path file, IOBuffer buffer, bool append, EIOPriority priority) = 0;
};

}// namespace atlas
