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

    virtual void initialize() = 0;
    virtual void deinitialize() = 0;
    virtual void tick(float delta_time) {};

    virtual std::shared_ptr<ApplicationWindow> make_window(const WindowDescription& description, const ApplicationWindow* parent) { return nullptr; }

    /**
    * @brief Make a non-visible, message-only dummy window for example used for GL.
     * @return
     */
    virtual std::shared_ptr<ApplicationWindow> make_dummy_window() { return nullptr; }

    virtual std::shared_ptr<ApplicationWindow> get_key_window() const { return nullptr; };
};

}// namespace atlas