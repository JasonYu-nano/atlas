// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "string/locale.hpp"

namespace atlas::locale
{

namespace
{
std::locale DefaultLocaleImpl()
{
    boost::locale::generator gen;
    return gen("en_US.UTF-8");
}
}

const std::locale& DefaultLocale()
{
    static std::locale default_locale = DefaultLocaleImpl();
    return default_locale;
}
}
