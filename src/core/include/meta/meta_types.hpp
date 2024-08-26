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
    Property                    = 1 << 3,
    BooleanProperty             = 1 << 4,
    NumericProperty             = 1 << 5,
    IntegralProperty            = 1 << 6,
    UnsignedIntegralProperty    = 1 << 7,
    FloatPointProperty          = 1 << 8,
    EnumProperty                = 1 << 9,
    EnumFieldProperty           = 1 << 10,
    StringProperty              = 1 << 11,
    StringNameProperty          = 1 << 12,
    ArrayProperty               = 1 << 13,
};
ENUM_BIT_MASK(EMetaCastFlag);

enum class EMetaFlag : uint32
{
    None            = 0,
    Serializable    = 1 << 0,
};

ENUM_BIT_MASK(EMetaFlag);

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

    NODISCARD bool has_flag(EMetaFlag flag) const
    {
        return test_flags(flags_, flag);
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
            meta_data_= new Map<StringName, variant_type>();
        }
        meta_data_->insert(key, variant_type{value});
    }

    void set_meta(StringName key, StringView value)
    {
        if (!meta_data_)
        {
            meta_data_= new Map<StringName, variant_type>();
        }
        meta_data_->insert(key, variant_type{String(value)});
    }
#endif

protected:
    void set_meta_flags(EMetaFlag flags) { flags_ = flags;  }

    EMetaFlag flags_{ EMetaFlag::None };
    StringName name_;
#if WITH_EDITOR
    Map<StringName, variant_type>* meta_data_{ nullptr };
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

}// namespace atlas
