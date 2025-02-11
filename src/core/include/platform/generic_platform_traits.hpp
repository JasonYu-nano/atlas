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

/**
 * @class GenericPlatformTraits
 * @brief Provides platform-specific traits and utility functions.
 */
class CORE_API GenericPlatformTraits
{
public:
    /**
     * @brief Loads a library from the specified path.
     * @param path The path to the library.
     * @return A handle to the loaded library, or nullptr if the load fails.
     */
    static void* load_library(const Path& path) VIRTUAL_IMPL(core, return nullptr;)

    /**
     * @brief Frees the specified library.
     * @param module_handle The handle to the library to be freed.
     */
    static void free_library(void* module_handle) VIRTUAL_IMPL(core)

    /**
     * @brief Retrieves an exported symbol from the specified library.
     * @param handle The handle to the library.
     * @param symbol_name The name of the symbol to retrieve.
     * @return A pointer to the symbol, or nullptr if the symbol is not found.
     */
    static void* get_exported_symbol(void* handle, const String& symbol_name) VIRTUAL_IMPL(core, return nullptr;)

    /**
     * @brief Gets the engine directory path.
     * @return The path to the engine directory.
     */
    static const Path& get_engine_directory();

    /**
     * @brief Gets the relative build directory path.
     * @return The path to the relative build directory.
     */
    static const Path& get_relative_build_directory();

    /**
     * @brief Constructs the library path from the module directory and library name.
     * @param module_dir The directory of the module.
     * @param lib_name The name of the library.
     * @return The constructed library path.
     */
    static Path get_library_path(const Path& module_dir, StringName lib_name);

    /**
     * @brief Sets the name of the specified thread.
     * @param thread_handle The handle to the thread.
     * @param name The name to set for the thread.
     */
    static void set_thread_name(void* thread_handle, const String& name) VIRTUAL_IMPL(core)

    /**
     * @brief Gets the handle of the current thread.
     * @return The handle of the current thread, or nullptr if the operation fails.
     */
    static void* get_this_thread_handle() VIRTUAL_IMPL(core, return nullptr;)

    /**
     * @brief Displays a message box with the specified type, caption, and message.
     * @param type The type of the message box.
     * @param caption The caption of the message box.
     * @param message The message to display in the message box.
     * @return The user's response to the message box.
     */
    static ESystemMsgBoxReturnType show_message_box(ESystemMsgBoxType type, const String& caption, const String& message) VIRTUAL_IMPL(core, return ESystemMsgBoxReturnType::No;)

    /**
     * @brief Sets up the crash handler for the platform.
     */
    static void setup_crash_handler() VIRTUAL_IMPL(core)

    /**
     * @brief Retrieves a GUID.
     * @param guid The GUID to be retrieved.
     */
    static void get_guid(class GUID& guid) VIRTUAL_IMPL(core)

    /**
     * @brief The alias name for the platform.
     */
    static inline const char* alias_name_ = "unknown";

    GenericPlatformTraits() = delete;
};

}// namespace atlas