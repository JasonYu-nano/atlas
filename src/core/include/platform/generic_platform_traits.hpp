// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_log.hpp"
#include "file_system/path.hpp"
#include "string/string_name.hpp"

namespace atlas
{

enum class ESystemMsgBoxType
{
    Ok,
    YesNo,
};

enum class ESystemMsgBoxReturnType
{
    Ok,
    Yes,
    No,
};

class CORE_API GenericPlatformTraits
{
public:
    static void* load_library(const Path& path) VIRTUAL_IMPL(core, return nullptr;)
    static void free_library(void* module_handle) VIRTUAL_IMPL(core)
    static void* get_exported_symbol(void* handle, const String& symbol_name) VIRTUAL_IMPL(core, return nullptr;)
    static const Path& get_engine_directory();
    static const Path& get_relative_build_directory();
    static Path get_library_path(const Path& module_dir, StringName lib_name);
    static void set_thread_name(void* thread_handle, const String& name) VIRTUAL_IMPL(core)
    static void* get_this_thread_handle() VIRTUAL_IMPL(core, return nullptr;)
    static ESystemMsgBoxReturnType show_message_box(ESystemMsgBoxType type, const String& caption, const String& message) VIRTUAL_IMPL(core, return ESystemMsgBoxReturnType::No;)
    static void setup_crash_handler() VIRTUAL_IMPL(core)

    static inline const char* alias_name_ = "unknown";

    GenericPlatformTraits() = delete;
};

}// namespace atlas