// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "application_implement.hpp"
#include "container/array.hpp"

namespace atlas
{

class APPLICATION_API MacApplication : public ApplicationImplement
{
public:
    void initialize() override;
    void deinitialize() override;
    void tick(float delta_time) override;

    /**
     * @brief create a native window instance.
     * @return
     */
    std::shared_ptr<ApplicationWindow> make_window(const WindowDescription& description, const ApplicationWindow* parent = nullptr) override;

protected:
    void on_window_destroyed(std::shared_ptr<ApplicationWindow> window);

    DelegateHandle windows_destroy_handle_;
    std::weak_ptr<ApplicationWindow> primary_window_;
    Array<std::shared_ptr<ApplicationWindow>> managed_windows;
};

} // namespace atlas