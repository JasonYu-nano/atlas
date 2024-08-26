// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "test_types.gen.hpp"
#include "meta/registration.hpp"
#include "test_types.hpp"

using namespace atlas;
using namespace atlas::test;

MetaClass* private_get_meta_class_MyStruct()
{
    return Registration::ClassReg<MyStruct>("MyStruct", EMetaFlag::None)
    .add_property(Registration::PropertyReg<bool>("b", OFFSET_OF(MyStruct, b)).get())
    .add_property(Registration::PropertyReg<int32>("id", OFFSET_OF(MyStruct, id)).get())
    .add_property(Registration::PropertyReg<float>("f", OFFSET_OF(MyStruct, f)).get())
    .add_property(Registration::PropertyReg<EMyEnum>("enumerator", OFFSET_OF(MyStruct, enumerator)).get())
    .add_property(Registration::PropertyReg<String>("str", OFFSET_OF(MyStruct, str)).get())
    .add_property(Registration::PropertyReg<StringName>("name", OFFSET_OF(MyStruct, name)).get())
    .add_method(Registration::MethodReg(&MyStruct::get_static_id, "get_static_id")
        .add_method_flags(EMethodFlag::Static)
        .get())
    .add_method(Registration::MethodReg(&MyStruct::get_id, "get_id")
        .get())
    .add_method(Registration::MethodReg(&MyStruct::add, "add")
        .add_method_flags(EMethodFlag::Static)
        .add_parameter(Registration::PropertyReg<int32>("a", 0).get())
        .add_parameter(Registration::PropertyReg<double>("b", 0).get())
        .get())
    .get();
}

template<>
MetaClass* meta_class_of<MyStruct>()
{
    static MetaClass* m = private_get_meta_class_MyStruct();
    return m;
}