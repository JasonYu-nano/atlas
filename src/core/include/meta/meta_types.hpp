// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <type_traits>
#include <variant>

#include "container/array.hpp"
#include "container/map.hpp"
#include "core_def.hpp"
#include "core_macro.hpp"
#include "log/logger.hpp"
#include "string/string_name.hpp"

namespace atlas
{

DEFINE_LOGGER(meta);

// Flags used to cast safely from MetaType.
enum class EMetaCastFlag : uint32
{
    MetaType                    = 0,
    Class                       = 1 << 0,
    Enum                        = 1 << 1,
    Method                      = 1 << 2,
    EnumField                   = 1 << 3,
    Property                    = 1 << 4,
    BooleanProperty             = 1 << 5,
    NumericProperty             = 1 << 6,
    IntegralProperty            = 1 << 7,
    UnsignedIntegralProperty    = 1 << 8,
    FloatPointProperty          = 1 << 9,
    EnumProperty                = 1 << 10,
    StringProperty              = 1 << 11,
    StringNameProperty          = 1 << 12,
    ClassProperty               = 1 << 13,
    PointerProperty             = 1 << 14,
    ArrayProperty               = 1 << 15,
};
ENUM_BIT_MASK(EMetaCastFlag);

/**
 * @class MetaType
 * @brief A base class for all meta types in the Atlas framework.
 *
 * This class provides the basic interface and functionality for meta types,
 * including type casting, metadata management, and name retrieval.
 */
class CORE_API MetaType
{
public:
    using variant_type = std::variant<std::monostate, int32, String>;

    virtual ~MetaType()
    {
        if (meta_data_ != nullptr)
        {
            delete meta_data_;
            meta_data_ = nullptr;
        }
    }

    /**
     * @brief Gets the cast flags for the MetaType.
     * Overload in child class.
     * @return The cast flags for the MetaType.
     */
    static EMetaCastFlag get_cast_flags()
    {
        return EMetaCastFlag::MetaType;
    }

    /**
     * @brief Checks if the current meta-type can be converted to the given type.
     * @tparam T The type to check against.
     * @return True if the meta-type can be converted to the given type, false otherwise.
     */
    template<typename T> requires std::is_base_of_v<MetaType, T>
    NODISCARD bool is() const
    {
        return test_flags(cast_flags(), T::get_cast_flags());
    }

    /**
     * @brief Gets the cast flags for the current instance.
     * @return The cast flags for the current instance.
     */
    NODISCARD virtual EMetaCastFlag cast_flags() const
    {
        return get_cast_flags();
    }

    /**
     * @brief Gets the name of the meta-type.
     * @return The name of the meta-type.
     */
    NODISCARD StringName name() const
    {
        return name_;
    }

#if WITH_EDITOR
    /**
     * @brief Gets the metadata associated with a given key.
     * @param key The key for the metadata.
     * @return The metadata associated with the key, or an empty variant if not found.
     */
    NODISCARD variant_type get_meta(StringName key) const
    {
        if (meta_data_)
        {
            const variant_type* value = meta_data_->find_value(key);
            if (value)
            {
                return *value;
            }
        }
        return {};
    }

    /**
     * @brief Sets the metadata for a given key to an integer value.
     * @param key The key for the metadata.
     * @param value The integer value to set.
     */
    void set_meta(StringName key, int32 value)
    {
        if (!meta_data_)
        {
            meta_data_= new UnorderedMap<StringName, variant_type>();
        }
        meta_data_->insert(key, variant_type{value});
    }

    /**
     * @brief Sets the metadata for a given key to a string value.
     * @param key The key for the metadata.
     * @param value The string value to set.
     */
    void set_meta(StringName key, StringView value)
    {
        if (!meta_data_)
        {
            meta_data_= new UnorderedMap<StringName, variant_type>();
        }
        meta_data_->insert(key, variant_type{String(value)});
    }
#endif

protected:
    StringName name_;
#if WITH_EDITOR
    UnorderedMap<StringName, variant_type>* meta_data_{ nullptr };
#endif
};


/**
 * @brief Casts a MetaType pointer to a pointer of type T if possible.
 *
 * This function template attempts to cast a MetaType pointer to a pointer of type T.
 * The cast is performed only if the MetaType can be converted to the given type T.
 *
 * @tparam T The type to which the MetaType pointer should be cast.
 * @param from A pointer to the MetaType to be cast.
 * @return A pointer to the type T if the cast is successful, nullptr otherwise.
 */
template<typename T> requires std::is_base_of_v<MetaType, T>
T* meta_cast(MetaType* from)
{
    return from->is<T>() ? static_cast<T*>(from) : nullptr;
}

#define DECLARE_META_CAST_FLAG(flag, base_type) \
public: \
friend class Registration; \
static EMetaCastFlag get_cast_flags() { return flag | base_type::get_cast_flags(); } \
NODISCARD EMetaCastFlag cast_flags() const override { return get_cast_flags(); } \
private:


using ParamPack = InlineArray<void*, 20>;

inline ParamPack param_pack_null = ParamPack();

/**
 * @brief Packs a variable number of arguments into a ParamPack.
 *
 * This function template takes a variable number of arguments and packs them into a ParamPack.
 * Each argument is cast to a void pointer and added to the ParamPack.
 *
 * @tparam Args The types of the arguments to be packed.
 * @param args The arguments to be packed.
 * @return A ParamPack containing the packed arguments.
 */
template<typename... Args>
ParamPack pack_arguments(const Args&... args)
{
    ParamPack package;
    (package.emplace(const_cast<Args*>(&args)), ...);
    return package;
}

#define FILE_PATH 1

#define FILE_ID_INTERNAL(path, class_name) path##_##class_name
#define FILE_ID_COMBINE(path, class_name) FILE_ID_INTERNAL(path, class_name)
#define FILE_ID(class_name) FILE_ID_COMBINE(FILE_PATH, class_name)

#define META_CODE_INTERNAL(file_id) META_CODE_##file_id
#define META_CODE_COMBINE(file_id) META_CODE_INTERNAL(file_id)

#if !defined(ATLAS_BUILDER)
#define GEN_META_CODE(class_name) META_CODE_COMBINE(FILE_ID(class_name))()
#else
#define GEN_META_CODE(class_name)
#endif

}// namespace atlas