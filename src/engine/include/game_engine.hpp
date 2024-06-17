// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "engine.hpp"

namespace atlas
{

class ENGINE_API GameEngine : public Engine
{
    using base = Engine;
public:
    GameEngine() : base() {}

    ~GameEngine() override = default;

    void startup(int argc, char** argv) override;
    void shutdown() override;
    void loop() override;

private:
    IManualTickableModule* application_module_;
};

} // namespace atlas