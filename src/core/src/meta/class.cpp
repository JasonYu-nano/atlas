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

}// namespace atlas