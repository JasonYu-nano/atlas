// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <type_traits>

#include "container/array.hpp"
#include "core_def.hpp"
#include "core_macro.hpp"

namespace atlas
{

enum class EMetaFlag : uint32
{
    None            = 0,
    Serializable    = 1 << 0,
};

ENUM_BIT_MASK(EMetaFlag);

class CORE_API MetaType
{
public:
    NODISCARD bool has_flag(EMetaFlag flag) const
    {
        return test_flags(flags_, flag);
    }

protected:
    EMetaFlag flags_{ EMetaFlag::None };
};

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
