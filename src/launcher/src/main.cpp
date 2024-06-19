// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "game_engine.hpp"

using namespace atlas;

int main(int argc, char* argv[])
{
    g_engine = new GameEngine();

    g_engine->startup(argc, argv);

    while (!g_engine->is_shutdown_requested())
    {
        g_engine->loop();
    }

    g_engine->shutdown();

    delete g_engine;
    g_engine = nullptr;

    return 0;
}