// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>

#include "application_window.hpp"

namespace atlas
{

class APPLICATION_API ApplicationImplement
{
public:
    virtual ~ApplicationImplement() = default;

    virtual void Initialize() = 0;
    virtual void Deinitialize() = 0;
    virtual void Tick(float delta_time) {};

    virtual std::shared_ptr<ApplicationWindow> MakeWindow(const WindowDescription& description, const ApplicationWindow* parent) { return nullptr; }

    /**
    * @brief Make a non-visible, message-only dummy window for example used for GL.
     * @return
     */
    virtual std::shared_ptr<ApplicationWindow> MakeDummyWindow() { return nullptr; }

    virtual std::shared_ptr<ApplicationWindow> GetKeyWindow() const { return nullptr; };
};

}// namespace atlas