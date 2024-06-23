// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "application_implement.hpp"

namespace atlas
{

class APPLICATION_API PlatformApplication
{
public:
    static std::unique_ptr<PlatformApplication> Create();

    PlatformApplication() = delete;

    void initialize()
    {
        implement_->initialize();
    }

    void deinitialize()
    {
        implement_->deinitialize();
    }

    void tick(float delta_time)
    {
        implement_->tick(delta_time);
    }

    std::shared_ptr<ApplicationWindow> make_window(const WindowDescription& description, const ApplicationWindow* parent = nullptr)
    {
        return implement_->make_window(description, parent);
    }

    std::shared_ptr<ApplicationWindow> make_dummy_window()
    {
        return implement_->make_dummy_window();
    }

    NODISCARD std::shared_ptr<ApplicationWindow> get_key_window() const
    {
        return implement_->get_key_window();
    }

private:
    explicit PlatformApplication(ApplicationImplement* implement) : implement_(implement) {}

    std::unique_ptr<ApplicationImplement> implement_{ nullptr };
};

}// namespace atlas