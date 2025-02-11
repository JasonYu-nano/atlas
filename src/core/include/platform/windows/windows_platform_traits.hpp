// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "platform/generic_platform_traits.hpp"

namespace atlas
{

class CORE_API WindowsPlatformTraits : public GenericPlatformTraits
{
public:
    WindowsPlatformTraits() = delete;

    /**
     * @brief Loads a library from the specified path.
     * @param path The path to the library.
     * @return A handle to the loaded library, or nullptr if the load fails.
     */
    static void* load_library(const Path& path);

    /**
     * @brief Frees the specified library.
     * @param module_handle The handle to the library to be freed.
     */
    static void free_library(void* module_handle);

    /**
     * @brief Retrieves an exported symbol from the specified library.
     * @param handle The handle to the library.
     * @param symbol_name The name of the symbol to retrieve.
     * @return A pointer to the symbol, or nullptr if the symbol is not found.
     */
    static void* get_exported_symbol(void* handle, const String& symbol_name);

    /**
     * @brief Sets the name of the specified thread.
     * @param thread_handle The handle to the thread.
     * @param name The name to set for the thread.
     */
    static void set_thread_name(void* thread_handle, const String& name);

    /**
     * @brief Gets the handle of the current thread.
     * @return The handle of the current thread, or nullptr if the operation fails.
     */
    static void* get_this_thread_handle();

    /**
     * @brief Displays a message box with the specified type, caption, and message.
     * @param type The type of the message box.
     * @param caption The caption of the message box.
     * @param message The message to display in the message box.
     * @return The user's response to the message box.
     */
    static ESystemMsgBoxReturnType show_message_box(ESystemMsgBoxType type, const String& caption, const String& message);

    /**
     * @brief Sets up the crash handler for the platform.
     */
    static void setup_crash_handler();

    /**
     * @brief Retrieves a GUID.
     * @param guid The GUID to be retrieved.
     */
    static void get_guid(class GUID& guid);

    /**
     * @brief The alias name for the platform.
     */
    static inline const char* alias_name_ = "windows";
};

using PlatformTraits = WindowsPlatformTraits;

} // namespace atlas