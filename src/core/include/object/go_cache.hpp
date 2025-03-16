// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "object/go.hpp"
#include "object_ptr.hpp"

namespace atlas
{

class CORE_API GOCache
{
public:
    void add(ObjectPtr<GObject> go)
    {
        if (nullptr != go)
        {
            std::unique_lock lock(cache_mutex_);
            cache_.insert(go->get_guid(), go);
        }
    }

    void remove(ObjectPtr<GObject> go)
    {
        if (nullptr != go)
        {
            std::unique_lock lock(cache_mutex_);
            cache_.remove(go->get_guid());
        }
    }

    void remove(Guid guid)
    {
        std::unique_lock lock(cache_mutex_);
        cache_.remove(guid);
    }

private:
    UnorderedMap<Guid, WeakObjectPtr<GObject>> cache_;
    std::shared_mutex cache_mutex_;
};

}// namespace atlas