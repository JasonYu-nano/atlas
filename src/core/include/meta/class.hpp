// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "constructor.hpp"
#include "meta/method.hpp"
#include "meta/property.hpp"
#include "meta_types.hpp"
#include "string/string_name.hpp"

namespace atlas
{

/**
 * @brief The meta-type of class and struct.
 * Use T::get_class() or instance.meta_class() for get metaclass.
 */
class CORE_API MetaClass : public MetaType
{
    using base = MetaType;
    DECLARE_META_CAST_FLAG(EMetaCastFlag::Class, base)
    friend class Registration;
public:
    MetaClass(uint32 size, Constructor* constructor) : size_(size), constructor_(constructor) {};

    NODISCARD bool is_derived_from(MetaClass* meta_class) const;

    NODISCARD bool is_object() const;

    NODISCARD uint32 class_size() const
    {
        return size_;
    }

    NODISCARD MetaClass* base_class() const;

    void construct(void* memory) const
    {
        if (constructor_)
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
    uint32 size_{ 0 };
    Constructor* constructor_{ nullptr };
    Array<Property*> properties_{};
    Array<Method*> methods_{};
};

class CORE_API MetaEnum : public MetaType
{
    using base = MetaType;
    DECLARE_META_CAST_FLAG(EMetaCastFlag::Class, base)
public:
    NODISCARD size_t size() const
    {
        return fields_.size();
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
        return 0;
    }

    NODISCARD StringName get_name_by_value(int64 value) const
    {
        return "";
    }

private:
    Array<EnumFieldProperty*> fields_{};
};

}// namespace atlas

template<typename T>
atlas::MetaClass* meta_class_of()
{
    return nullptr;
}