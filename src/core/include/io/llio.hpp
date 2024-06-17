// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "io/io_backend_interface.hpp"
#include "misc/delegate_fwd.hpp"

namespace atlas
{

/**
 * @brief LowLevelIO provides fast asynchronous file read and write interfaces.
 */
class CORE_API LowLevelIO
{
public:
    LowLevelIO()
    {
        create_io_backend();
    }

    ~LowLevelIO()
    {
        delete io_backend_;
        io_backend_ = nullptr;
    }
    /**
     * @brief Asynchronously reads the given file.
     * @param file
     * @param priority 
     * @return 
     */
    Task<IOBuffer> async_read(Path file, EIOPriority priority = EIOPriority::Normal)
    {
        IOBuffer buffer;
        co_await io_backend_->async_read(std::move(file), buffer, INDEX_NONE, 0, priority);
        co_return buffer;
    }
    /**
     * @brief Asynchronously reads the given file.
     * @param file
     * @param read_size
     * @param priority
     * @return
     */
    Task<IOBuffer> async_read(Path file, size_t read_size, EIOPriority priority = EIOPriority::Normal)
    {
        IOBuffer buffer;
        co_await io_backend_->async_read(std::move(file), buffer, read_size, 0, priority);
        co_return buffer;
    }
    /**
     * @brief Asynchronously reads the given file.
     * @param file
     * @param offset
     * @param read_size
     * @param priority
     * @return
     */
    Task<IOBuffer> async_read(Path file, size_t offset, size_t read_size = INDEX_NONE, EIOPriority priority = EIOPriority::Normal)
    {
        IOBuffer buffer;
        co_await io_backend_->async_read(std::move(file), buffer, read_size, offset, priority);
        co_return buffer;
    }

    /** Asynchronously reads the file and appends to the given buffer.
     * @brief
     * @param file
     * @param buffer
     * @param priority
     * @return
     */
    Task<size_t> async_read(Path file, IOBuffer& buffer, EIOPriority priority = EIOPriority::Normal)
    {
        return io_backend_->async_read(std::move(file), buffer, INDEX_NONE, 0, priority);
    }
    /** Asynchronously reads the file and appends to the given buffer.
     * @brief
     * @param file
     * @param buffer
     * @param read_size
     * @param priority
     * @return
     */
    Task<size_t> async_read(Path file, IOBuffer& buffer, size_t read_size, EIOPriority priority = EIOPriority::Normal)
    {
        return io_backend_->async_read(std::move(file), buffer, read_size, 0, priority);
    }
    /** Asynchronously reads the file and appends to the given buffer.
     * @brief
     * @param file
     * @param buffer
     * @param offset
     * @param read_size
     * @param priority
     * @return
     */
    Task<size_t> async_read(Path file, IOBuffer& buffer, size_t offset, size_t read_size = INDEX_NONE, EIOPriority priority = EIOPriority::Normal)
    {
        return io_backend_->async_read(std::move(file), buffer, read_size, offset, priority);
    }
    /** Asynchronously writes the given buffer to file.
     * @brief
     * @param file
     * @param buffer
     * @param append
     * @param priority
     * @return
     */
    Task<size_t> async_write(Path file, IOBuffer buffer, bool append = false, EIOPriority priority = EIOPriority::Normal)
    {
        return io_backend_->async_write(std::move(file), buffer, append, priority);
    }

private:
    void create_io_backend();

    IIOBackend* io_backend_{ nullptr };
};

}// namespace atlas
