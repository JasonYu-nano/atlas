// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <fstream>
#include <stdio.h>

#include "io/filesystem_io_backend.hpp"

#include "async/schedule_on.hpp"
#include "configuration/config_manager.hpp"
#include "log/logger.hpp"
#include "utility/on_scope_exit.hpp"

namespace atlas
{

/** Num of io worker. */
uint32 g_io_worker_count = 2;
ConfigVariableRefRegister tmp("io", "filesystem_io_worker_count", g_io_worker_count);

Task<size_t> FilesystemIOBackend::async_read(Path file, IOBuffer& buffer, size_t read_size, size_t offset, EIOPriority priority)
{
    size_t read = 0;
    if (!std::filesystem::exists(file))
    {
        co_return read;
    }

    FILE* stream = fopen(file.to_string().data(), "r");
    if (!stream)
    {
        LOG_WARN(core, "Failed to open file {0}", file);
        co_return read;
    }

    auto guard = on_scope_exit([=]() {
        fclose(stream);
    });

    fseek(stream, 0, SEEK_END);
    size_t size = ftell(stream);

    if (offset >= size)
    {
        LOG_WARN(core, "File offset over file size");
        guard.call_now();
        co_return read;
    }

    size_t actual_read_size = read_size == INDEX_NONE ? size : math::min(read_size, size - offset);
    if (actual_read_size <= 0)
    {
        guard.call_now();
        co_return read;
    }

    const size_t old_size = buffer.size();
    if (buffer.max_size() - old_size < actual_read_size)
    {
        LOG_WARN(core, "IO buffer is not enough.");
        guard.call_now();
        co_return read;
    }

    buffer.resize(old_size + actual_read_size);

    co_await co_schedule_on(*this, priority);

    fseek(stream, offset, SEEK_SET);
    read = std::fread(buffer.data() + old_size, sizeof(byte), actual_read_size, stream);

    if (read != actual_read_size)
    {
        if (!feof(stream))
        {
            LOG_WARN(core, "Read file {} failed", file);
            buffer.resize(old_size);
        }
        else
        {
            buffer.resize(read);
        }
    }

    guard.call_now();
    co_return read;
}

Task<size_t> FilesystemIOBackend::async_write(Path file, IOBuffer buffer, bool append, EIOPriority priority)
{
    size_t write = 0;
    if (!std::filesystem::exists(file))
    {
        co_return write;
    }

    FILE* stream = fopen(file.to_string().data(), append ? "a" : "w");
    if (!stream)
    {
        LOG_WARN(core, "Failed to open file {0}", file);
        co_return write;
    }

    auto guard = on_scope_exit([=]() {
        fclose(stream);
    });

    co_await co_schedule_on(*this, priority);

    write = fwrite(buffer.data(), sizeof(byte), buffer.size(), stream);

    guard.call_now();
    co_return write;
}

uint32 FilesystemIOBackend::get_io_worker_count()
{
    return g_io_worker_count;
}

}// namespace atlas