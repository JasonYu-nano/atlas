// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "meta/class.hpp"

namespace atlas
{

bool MetaClass::is_derived_from(MetaClass* meta_class) const
{
    return false;
}

bool MetaClass::is_object() const
{
    return false;
}

MetaClass* MetaClass::base_class() const
{
    return nullptr;
}

void MetaClass::construct(void* memory) const
{
}

void MetaClass::destruct(void* memory) const
{
}

Property* MetaClass::get_property(StringName name) const
{
    return nullptr;
}

Method* MetaClass::get_method(StringName name) const
{
    return nullptr;
}

}// namespace atlas