// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "io/llio.hpp"
#include "io/filesystem_io_backend.hpp"

void atlas::LowLevelIO::create_io_backend()
{
    if (io_backend_)
    {
        delete io_backend_;
        io_backend_ = nullptr;
    }

    io_backend_ = new FilesystemIOBackend();
}