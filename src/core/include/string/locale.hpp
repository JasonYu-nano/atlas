// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "boost/locale/generator.hpp"
#include "boost/locale/conversion.hpp"

#include "core_def.hpp"

namespace atlas::locale
{

CORE_API std::locale DefaultLocale();

inline std::locale DefaultLocale()
{
    boost::locale::generator generator;
    return generator("en_US.UTF-8");
}

}
