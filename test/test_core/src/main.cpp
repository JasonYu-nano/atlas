// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"
#include "meta/registration.hpp"

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    atlas::Registration::register_meta_types();
    auto ret = RUN_ALL_TESTS();

    return ret;
}