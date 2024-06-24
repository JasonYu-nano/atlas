// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "file_system/path.hpp"
#include "string/string_name.hpp"

namespace atlas
{

class CORE_API GenericPlatformTraits
{
public:
    static void* load_library(const Path& path) { return nullptr; }
    static void free_library(void* module_handle) {}
    static void* get_exported_symbol(void* handle, const String& symbol_name) { return nullptr; }
    static const Path& get_engine_directory();
    static const Path& get_relative_build_directory();
    static Path get_library_path(const Path& module_dir, StringName lib_name);
    static void set_thread_name(void* thread_handle, const String& name) {};
    static void* get_this_thread_handle() { return nullptr; }

    GenericPlatformTraits() = delete;
};

}// namespace atlas