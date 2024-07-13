// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "async/schedule_on.hpp"
#include "async/static_thread_pool.hpp"
#include "io_backend_interface.hpp"

namespace atlas
{

class FilesystemIOBackend : public IIOBackend
{
public:
    using awaiter_type = ScheduleAwaiter<FilesystemIOBackend, EIOPriority>;

    FilesystemIOBackend()
        : thread_pool_(get_io_worker_count(), "io thread")
    {}

    ~FilesystemIOBackend() override = default;

    void schedule(std::coroutine_handle<> handle, EIOPriority priority)
    {
        thread_pool_.push_task(static_cast<uint32>(priority), [=] { handle.resume(); });
    }

    Task<size_t> async_read(Path file, IOBuffer& buffer, size_t read_size, size_t offset, EIOPriority priority) override;

    Task<size_t> async_write(Path file, IOBufferView buffer, bool append, EIOPriority priority) override;

    static uint32 get_io_worker_count();

private:
    StaticThreadPool<g_io_priority_count> thread_pool_;
};

}// namespace atlas