// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "game_engine.hpp"

using namespace atlas;

int main()
{
    g_engine = new GameEngine();

    g_engine->Startup();

    while (!g_engine->IsShutdownRequested())
    {
        g_engine->Loop();
    }

    g_engine->Shutdown();

    delete g_engine;
    g_engine = nullptr;

    return 0;
}