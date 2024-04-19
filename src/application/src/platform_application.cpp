// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "platform_application.hpp"
#include "assertion.hpp"

#if PLATFORM_WINDOWS
#include "platform/windows/windows_application.hpp"
#elif PLATFORM_APPLE
#include "platform/mac/mac_application.hpp"
#endif

namespace atlas
{

std::unique_ptr<PlatformApplication> PlatformApplication::Create()
{
    PlatformApplication* app;
#if PLATFORM_WINDOWS
    app = new PlatformApplication(new WindowsApplication());
#elif PLATFORM_APPLE
    app = new PlatformApplication(new MacApplication());
#endif
    ASSERT(app);
    return std::unique_ptr<PlatformApplication>(app);
}

}// namespace atlas