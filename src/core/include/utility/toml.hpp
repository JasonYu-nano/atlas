// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "toml++/toml.hpp"

namespace toml
{

CORE_API inline node_type get_homogeneous_type(const array& a)
{
    node_type ntype = node_type::none;
    if (!a.empty())
    {
        ntype = a[0].type();
        for (const auto& node : a)
        {
            if (node.type() != ntype)
            {
                ntype = node_type::none;
                break;
            }
        }
    }
    
    return ntype;
}

}// namespace toml