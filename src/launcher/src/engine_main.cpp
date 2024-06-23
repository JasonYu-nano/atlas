// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "engine_main.hpp"
#include "game_engine.hpp"
#include "misc/on_scope_exit.hpp"

namespace atlas
{

int EngineMain(int argc, char* argv[])
{
    g_engine = new GameEngine();

    // make sure can always call engine shutdown on program terminated.
    auto guard = on_scope_exit([]{
        g_engine->shutdown();
    });

    g_engine->startup(argc, argv);

    while (!g_engine->is_shutdown_requested())
    {
        g_engine->loop();
    }

    g_engine->shutdown();

    guard.cancel();

    delete g_engine;
    g_engine = nullptr;

    return 0;
}

}// namespace atlas
