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
    void Initialize() override;
    void Deinitialize() override;
    void Tick(float delta_time) override;

    /**
     * @brief Create a native window instance.
     * @return
     */
    std::shared_ptr<ApplicationWindow> MakeWindow(const WindowDescription& description, const ApplicationWindow* parent = nullptr) override;

protected:
    void OnWindowDestroyed(std::shared_ptr<ApplicationWindow> window);

    DelegateHandle windows_destroy_handle_;
    std::weak_ptr<ApplicationWindow> primary_window_;
    Array<std::shared_ptr<ApplicationWindow>> managed_windows;
};

} // namespace atlas