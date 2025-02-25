// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <type_traits>

#include "core_def.hpp"

namespace atlas
{

class CORE_API Constructor
{
public:
    virtual ~Constructor() = default;

    virtual void construct(void* memory) const = 0;

    virtual void destruct(void* memory) const = 0;
};

template<typename T> requires std::is_default_constructible_v<T>
class TypeConstructor : public Constructor
{
public:
    ~TypeConstructor() override = default;

    void construct(void* memory) const override
    {
        new(memory) T();
    }

    void destruct(void* ptr) const override
    {
        static_cast<T*>(ptr)->~T();
    }
};

}// namespace atlas