// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "check.hpp"
#include "meta_types.hpp"

namespace atlas
{

enum class EMethodFlag : uint8
{
    None        = 0,
    Static      = 1 << 0,
    Const       = 1 << 0,
};

ENUM_BIT_MASK(EMethodFlag);

class CORE_API Method : public MetaType
{
public:
    using method_pointer = void(*)(void*, ParamPack&, void*);

    explicit Method(method_pointer p, EMethodFlag flags = EMethodFlag::None) : method_(p), flags_(flags) {}

    void invoke(ParamPack& param_pack, void* result) const
    {
        CHECK(method_, "Method instance is null");
        if (test_flags(flags_, EMethodFlag::Static))
        {
            method_(nullptr, param_pack, result);
        }
        else
        {
            LOG_ERROR(core, "Non-static methods are not allowed to be called without an object instance.")
        }
    }

    void invoke(void* instance, ParamPack& param_pack, void* result) const
    {
        CHECK(method_, "Method instance is null");
        if (instance || test_flags(flags_, EMethodFlag::Static))
        {
            method_(instance, param_pack, result);
        }
        else
        {
            LOG_ERROR(core, "Non-static methods are not allowed to be called without an object instance.")
        }
    }

    NODISCARD bool is_static() const
    {
        return test_flags(flags_, EMethodFlag::Static);
    }

    NODISCARD bool is_const() const
    {
        return test_flags(flags_, EMethodFlag::Const);
    }

protected:
    method_pointer method_{ nullptr };
    EMethodFlag flags_{ EMethodFlag::None };
};

}// namespace atlas