// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "meta/meta_types.hpp"
#include "object_ptr.hpp"
#include "utility/guid.hpp"
#include "go.gen.hpp"

namespace atlas
{

class CORE_API META() GObject
{
    GEN_META_CODE(GObject)
public:
    GObject() : guid_(Guid::new_guid()) {}

    virtual ~GObject() = default;

    META() Guid get_guid() const { return guid_; }

    void set_outer(ObjectPtr<GObject> outer) { outer_ = outer; }

private:
    META() Guid guid_;
    ObjectPtr<GObject> outer_;
};

template<typename T>
inline constexpr bool is_object_v = std::is_base_of_v<GObject, T>;

}// namespace atlas