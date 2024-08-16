// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "meta/method.hpp"
#include "meta/property.hpp"
#include "meta_types.hpp"
#include "string/string_name.hpp"

namespace atlas
{

/**
 * @brief The meta-type of class and struct.
 * Use T::class() or instance.get_class() for get metaclass.
 */
class CORE_API MetaClass : public MetaType
{
    using base = MetaType;
public:
    MetaClass() = default;

    NODISCARD bool is_derived_from(MetaClass* meta_class) const;

    NODISCARD bool is_object() const;

    NODISCARD uint32 class_size() const
    {
        return size_;
    }

    NODISCARD MetaClass* base_class() const;

    void construct(void* memory) const;

    void destruct(void* memory) const;

    NODISCARD Property* get_property(StringName name) const;

    NODISCARD Method* get_method(StringName name) const;

private:
    uint32 size_{ 0 };
};

}// namespace atlas