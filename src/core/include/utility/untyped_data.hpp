// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"

namespace atlas
{

template <size_t Size, uint32 Align>
struct AlignedBytes
{
    struct alignas(Align) PaddingType
    {
        uint8 pad[Size];
    };
    PaddingType padding;
};

template <size_t Size>
struct AlignedBytes<Size, 1>
{
    uint8 pad[Size];
};

template <typename T>
struct UntypedData : private AlignedBytes<sizeof(T), alignof(T)>
{
    using ValueType = T;
    ValueType* GetData() { return (ValueType*)this; }
    const ValueType* GetData() const { return (ValueType*)this; }
};

}
