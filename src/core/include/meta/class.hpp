// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "constructor.hpp"
#include "container/unordered_set.hpp"
#include "meta/method.hpp"
#include "meta/property.hpp"
#include "meta_types.hpp"
#include "string/string_name.hpp"

namespace atlas
{

enum class EMetaClassFlag : uint32
{
    None            = 0,
    Abstract        = 1 << 0,
    Interface       = 1 << 1,
};

ENUM_BIT_MASK(EMetaClassFlag);

enum class EMetaEnumFlag : uint32
{
    None            = 0,
};

ENUM_BIT_MASK(EMetaEnumFlag);

enum class EEnumUnderlyingType : uint8
{
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
};

/**
 * @brief The meta-type of class and struct.
 * Use meta_class_of<T> or instance.meta_class() for get metaclass.
 */
class CORE_API MetaClass : public MetaType
{
    using base = MetaType;
    DECLARE_META_CAST_FLAG(EMetaCastFlag::Class, base)
    friend class Registration;
public:
    static MetaClass* find_class(StringName name)
    {
        const auto it = meta_class_map_.find(name);
        return it != meta_class_map_.end() ? it->second.get() : nullptr;
    }

    static void foreach_class(std::function<void(MetaClass*)> fn)
    {
        std::for_each(meta_class_map_.begin(), meta_class_map_.end(), [&fn](auto&& it) {
            fn(it.second.get());
        });
    }

    MetaClass(uint32 size, uint32 align, Constructor* constructor) : size_(size), align_(align), constructor_(constructor) {}

    ~MetaClass() override
    {
        delete children_;
        children_ = nullptr;

        delete constructor_;
        constructor_ = nullptr;

        for (auto prop : properties_)
        {
            delete prop;
        }

        for (auto method : methods_)
        {
            delete method;
        }
    }

    NODISCARD MetaClass* base_class() const
    {
        return base_;
    }

    NODISCARD bool is_derived_from(MetaClass* meta_class) const;

    NODISCARD bool has_implement_interface(MetaClass* meta_class) const;

    NODISCARD bool has_child() const
    {
        return children_ != nullptr && !children_->empty();
    }

    NODISCARD UnorderedSet<MetaClass*> get_children(bool recursion = false) const;

    NODISCARD bool is_object() const;

    NODISCARD uint32 class_size() const
    {
        return size_;
    }

    NODISCARD bool has_flag(EMetaClassFlag flag) const
    {
        return test_flags(flags_, flag);
    }

    void construct(void* memory) const
    {
        if (constructor_ && !has_flag(EMetaClassFlag::Abstract))
        {
            constructor_->construct(memory);
        }
    }

    void destruct(void* ptr) const
    {
        if (constructor_)
        {
            constructor_->destruct(ptr);
        }
    }

    NODISCARD Property* get_property(StringName name) const
    {
        size_t idx = properties_.find([=](Property* prop) { return prop->name() == name; });
        return idx == INDEX_NONE ? nullptr : properties_[idx];
    }

    NODISCARD Method* get_method(StringName name) const
    {
        size_t idx = methods_.find([=](Method* method) { return method->name() == name; });
        return idx == INDEX_NONE ? nullptr : methods_[idx];
    }

private:
    static inline UnorderedMap<StringName, std::unique_ptr<MetaClass>> meta_class_map_;

    EMetaClassFlag flags_{ EMetaClassFlag::None };
    uint32 size_{ 0 };
    uint32 align_{ 0 };
    MetaClass* base_{ nullptr };
    UnorderedSet<MetaClass*>* children_{ nullptr };
    Constructor* constructor_{ nullptr };
    Array<MetaClass*> interfaces_{};
    Array<Property*> properties_{};
    Array<Method*> methods_{};
};

class CORE_API MetaEnum : public MetaType
{
    using base = MetaType;
    DECLARE_META_CAST_FLAG(EMetaCastFlag::Enum, base)
public:
    static MetaEnum* find_enum(StringName name)
    {
        const auto it = meta_enum_map_.find(name);
        return it != meta_enum_map_.end() ? it->second.get() : nullptr;
    }

    static void foreach_enum(std::function<void(MetaEnum*)> fn)
    {
        std::for_each(meta_enum_map_.begin(), meta_enum_map_.end(), [&fn](auto&& it) {
            fn(it.second.get());
        });
    }

    ~MetaEnum() override
    {
        for (auto field : fields_)
        {
            delete field;
        }
    }

    NODISCARD bool has_flag(EMetaEnumFlag flag) const
    {
        return test_flags(flags_, flag);
    }

    NODISCARD size_t size() const
    {
        return fields_.size();
    }

    NODISCARD EnumField* get_field(size_t index) const
    {
        CHECK(fields_.is_valid_index(index), "Index over bounds");
        return fields_[index];
    }

    NODISCARD int64 get_enum_value(size_t index) const
    {
        CHECK(fields_.is_valid_index(index), "Index over bounds");
        return fields_[index]->value();
    }

    NODISCARD StringName get_enum_name(size_t index) const
    {
        CHECK(fields_.is_valid_index(index), "Index over bounds");
        return fields_[index]->name();
    }

    NODISCARD int64 get_value_by_name(StringName name) const
    {
        for (auto field : fields_)
        {
            if (field->name() == name)
            {
                return field->value();
            }
        }
        return INDEX_NONE;
    }

    NODISCARD StringName get_name_by_value(int64 value) const
    {
        for (auto field : fields_)
        {
            if (field->value() == value)
            {
                return field->name();
            }
        }
        return "";
    }

private:
    static inline UnorderedMap<StringName, std::unique_ptr<MetaEnum>> meta_enum_map_;

    EEnumUnderlyingType underlying_type_{ EEnumUnderlyingType::Int32 };
    EMetaEnumFlag flags_{ EMetaEnumFlag::None };
    Array<EnumField*> fields_{};
};

}// namespace atlas

template<typename T>
atlas::MetaClass* meta_class_of()
{
    return nullptr;
}

template<typename T>
atlas::MetaEnum* meta_enum_of()
{
    return nullptr;
}