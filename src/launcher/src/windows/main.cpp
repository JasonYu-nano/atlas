// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "engine_main.hpp"
#include "platform/windows/windows_minimal_api.hpp"

using namespace atlas;

int main(int argc, char* argv[])
{
    int error_code = EngineMain(argc, argv);
    PostQuitMessage(error_code);
    return error_code;
}