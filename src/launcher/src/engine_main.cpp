// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "engine_main.hpp"
#include "game_engine.hpp"
#include "misc/scope_guard.hpp"

namespace atlas
{

int EngineMain(int argc, char* argv[])
{
    g_engine = new GameEngine();

    using exit = decltype([]() {
        g_engine->Shutdown();
    });

    // make sure can always call engine shutdown on program terminated.
    ScopeGuard<exit> guard;


    g_engine->Startup();

    while (!g_engine->IsShutdownRequested())
    {
        g_engine->Loop();
    }

    g_engine->Shutdown();

    guard.Release();

    delete g_engine;
    g_engine = nullptr;

    return 0;
}

}// namespace atlas
