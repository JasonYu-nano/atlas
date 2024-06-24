// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "io_backend_interface.hpp"
#include "async/schedule_on.hpp"
#include "async/thread.hpp"
#include "container/array.hpp"
#include "concurrency/priority_queue.hpp"

namespace atlas
{

class FilesystemIOBackend;

class IOTaskWorker
{
public:
    explicit IOTaskWorker(uint8 thread_id, FilesystemIOBackend* producer) : thread_id_(thread_id), producer_(producer) {}

    void operator() (StopToken stop_token);

private:
    uint8 thread_id_{ 0 };
    FilesystemIOBackend* producer_{ nullptr };
};

class FilesystemIOBackend : public IIOBackend
{
    friend IOTaskWorker;
public:
    using awaiter_type = ScheduleAwaiter<FilesystemIOBackend, EIOPriority>;

    FilesystemIOBackend()
    {
        for (uint32 i = 0; i < get_io_worker_count(); ++i)
        {
            running_threads_.emplace(IOTaskWorker(running_threads_.size(), this));
            running_threads_.last().set_name(String::format("io thread - {0}", running_threads_.size()));
        }
    }

    ~FilesystemIOBackend() override
    {
        for(auto&& thread : running_threads_)
        {
            thread.request_stop();
        }
        running_threads_.clear();
    }

    void schedule(std::coroutine_handle<> handle, EIOPriority priority);

    Task<size_t> async_read(Path file, IOBuffer& buffer, size_t read_size, size_t offset, EIOPriority priority) override;

    Task<size_t> async_write(Path file, IOBufferView buffer, bool append, EIOPriority priority) override;

    static uint32 get_io_worker_count();
private:
    Array<Thread> running_threads_;
    PriorityQueue<void, g_io_priority_count> request_queue_;
};

}// namespace atlas