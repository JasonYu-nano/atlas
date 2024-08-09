// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "engine.hpp"

namespace atlas
{

class EDITOR_API EditorEngine : public Engine
{
public:
    void startup(int argc, char** argv) override;

    void shutdown() override;
};

}// namespace atlas