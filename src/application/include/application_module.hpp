// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "module/module_interface.hpp"
#include "platform_application.hpp"

namespace atlas
{

class APPLICATION_API ApplicationModule : public IManualTickableModule
{
public:
    void startup() override;

    void shutdown() override;

    void tick(float delta_time) override;

    NODISCARD PlatformApplication* GetApplication() const
    {
        return application_.get();
    }

    NODISCARD PlatformApplication* GetApplication() const
    {
        return application_.get();
    }

private:
    std::unique_ptr<PlatformApplication> application_{ nullptr };
};

}// namespace atlas