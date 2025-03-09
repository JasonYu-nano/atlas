// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "object/go_allocator.hpp"
#include "object/go_cache.hpp"

namespace atlas
{

extern CORE_API GOAllocator g_go_allocator;

extern CORE_API GOCache g_go_cache;

namespace details
{

struct GODeleter
{
    void operator()(GObject* go)
    {
        g_go_cache.remove(go->get_guid());
        go->~GObject();
        g_go_allocator.deallocate(go);
        go = nullptr;
    }
};

}// namespace details

template<typename T> requires is_object_v<T>
ObjectPtr<T> new_object(ObjectPtr<T> owner)
{
    void* ptr = g_go_allocator.allocate(sizeof(T), alignof(T));

    ObjectPtr<T> go = ObjectPtr<T>(new (ptr) T(), details::GODeleter());
    go->set_outer(owner);
    g_go_cache.add(go);

    return go;
}

template<typename T> requires is_object_v<T>
ObjectPtr<T> new_object(MetaClass* cls, ObjectPtr<T> owner)
{
    auto class_of_t = meta_class_of<T>();
    CHECK(cls && (cls == class_of_t || cls->is_derived_from(class_of_t)), "cls \"{0}\" must be derived from type \"{1}\"", cls->name().to_string(), class_of_t->name().to_string());
    void* ptr = g_go_allocator.allocate(cls->class_size(), cls->class_align());
    cls->construct(ptr);

    ObjectPtr<T> go = ObjectPtr<T>(static_cast<T*>(ptr), details::GODeleter());
    go->set_outer(owner);
    g_go_cache.add(go);

    return go;
}

}// namespace atlas