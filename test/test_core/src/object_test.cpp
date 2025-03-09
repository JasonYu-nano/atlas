// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"
#include "object/go_fwd.hpp"

namespace atlas::test
{

TEST(AObjectest, GOConstruct)
{
    auto go1 = new_object<GObject>(nullptr);
    auto go2 = new_object<GObject>(meta_class_of<GObject>(), nullptr);
}

}// namespace atlas::test