// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "platform/generic_platform_traits.hpp"

namespace atlas
{

class CORE_API MacPlatformTraits : public GenericPlatformTraits
{
public:
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
     * @brief Constructs the library path from the module directory and library name.
     * @param module_dir The directory of the module.
     * @param lib_name The name of the library.
     * @return The constructed library path.
     */
    static Path get_library_path(const Path& module_dir, StringName lib_name);

    /**
     * @brief Sets the name of the current thread.
     * @param name The name to set for the thread.
     */
    static void set_thread_name(const String& name);

    /**
     * @brief Gets the handle of the current thread.
     * @return The handle of the current thread, or nullptr if the operation fails.
     */
    static void* get_this_thread_handle();

    /**
     * @brief Retrieves a GUID.
     * @param guid The GUID to be retrieved.
     */
    static void get_guid(class Guid& guid);

    MacPlatformTraits() = delete;
};

using PlatformTraits = MacPlatformTraits;

} // namespace atlas