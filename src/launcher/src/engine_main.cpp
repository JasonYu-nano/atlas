// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "engine_main.hpp"

#include "check.hpp"
#include "game_engine.hpp"
#include "utility/on_scope_exit.hpp"

namespace atlas
{

int EngineMain(int argc, char* argv[])
{
#if WITH_CRASH_HANDLER
    PlatformTraits::setup_crash_handler();
#endif

    g_engine = new GameEngine();
    CHECK(g_engine, "");

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