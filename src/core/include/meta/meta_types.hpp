// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <type_traits>
#include <variant>

#include "container/array.hpp"
#include "container/map.hpp"
#include "core_def.hpp"
#include "core_macro.hpp"
#include "string/string_name.hpp"

namespace atlas
{

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
    ArrayProperty               = 1 << 14,
};
ENUM_BIT_MASK(EMetaCastFlag);

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

    static EMetaCastFlag get_cast_flags()
    {
        return EMetaCastFlag::MetaType;
    }

    /**
     * @brief Checks if the current meta-type can be converted to the given type.
     * @tparam T
     * @return
     */
    template<typename T> requires std::is_base_of_v<MetaType, T>
    NODISCARD bool is() const
    {
        return test_flags(cast_flags(), T::get_cast_flags());
    }

    NODISCARD virtual EMetaCastFlag cast_flags() const
    {
        return get_cast_flags();
    }

    NODISCARD StringName name() const
    {
        return name_;
    }

#if WITH_EDITOR
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

    void set_meta(StringName key, int32 value)
    {
        if (!meta_data_)
        {
            meta_data_= new UnorderedMap<StringName, variant_type>();
        }
        meta_data_->insert(key, variant_type{value});
    }

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

template<typename... Args>
ParamPack pack_arguments(const Args&... args)
{
    ParamPack package;
    (package.emplace(const_cast<Args*>(&args)), ...);
    return package;
}

#if defined(ATLAS_BUILDER)
#define META(...) [[clang::annotate(#__VA_ARGS__)]]
#else
#define META(...)
#endif

#define FILE_PATH 1

#define FILE_ID_INTERNAL(path, class_name) path##_##class_name
#define FILE_ID_COMBINE(path, class_name) FILE_ID_INTERNAL(path, class_name)
#define FILE_ID(class_name) FILE_ID_COMBINE(FILE_PATH, class_name)

#define CLASS_BODY_INTERNAL(fileId) CLASS_BODY_##fileId
#define CLASS_BODY_COMBINE(fileId) CLASS_BODY_INTERNAL(fileId)

#if !defined(ATLAS_BUILDER)
#define GEN_CLASS_BODY(class_name) CLASS_BODY_COMBINE(FILE_ID(class_name))()
#else
#define GEN_CLASS_BODY(class_name)
#endif

}// namespace atlas