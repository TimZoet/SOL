#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-memory/fwd.h"
#include "sol-task/fwd.h"
#include "sol-window/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "base-application/tasks/non_owning_task_resource.h"
#include "base-application/tasks/pointer_task_resource.h"

class BaseApplication
{
public:
    BaseApplication();

    BaseApplication(const BaseApplication&) = delete;

    BaseApplication(BaseApplication&&) noexcept = delete;

    virtual ~BaseApplication() noexcept;

    BaseApplication& operator=(const BaseApplication&) = delete;

    BaseApplication& operator=(BaseApplication&&) noexcept = delete;

    [[nodiscard]] bool parse(int argc, char** argv);

    virtual void initialize();

    void run();

    virtual std::vector<std::string> getExtensions() = 0;

    virtual void createSupportedFeatures() = 0;

    virtual void createEnabledFeatures() = 0;

    virtual std::function<bool(sol::RootVulkanPhysicalDeviceFeatures2&)> getFeatureFilter() = 0;

    virtual void createTaskGraph() = 0;


protected:
    void createWindow();

    void createInstance();

    void createSurface();

    void createPhysicalDevice();

    void createDevice();

    void createSwapchain();

    void createMemoryManager();

    void createCommandPools();

    struct
    {
        uint32_t width     = 0;
        uint32_t height    = 0;
        uint32_t maxFrames = 2;
    } args;

    sol::VulkanPhysicalDeviceFeatures2Ptr supportedFeatures;
    sol::VulkanPhysicalDeviceFeatures2Ptr enabledFeatures;

    PointerTaskResource<uint32_t, false>            frameIndex{0};
    PointerTaskResource<uint32_t, false>            imageIndex{0};
    sol::WindowPtr                                  window;
    sol::VulkanInstancePtr                          instance;
    sol::VulkanSurfacePtr                           surface;
    sol::VulkanPhysicalDevicePtr                    physicalDevice;
    sol::VulkanDevicePtr                            device;
    PointerTaskResource<sol::VulkanSwapchain, true> swapchain;
    sol::MemoryManagerPtr                           memoryManager;
    sol::TaskGraphPtr                               taskGraph;
    NonOwningTaskResource<sol::VulkanQueue>         computeQueue;
    NonOwningTaskResource<sol::VulkanQueue>         graphicsQueue;

    struct
    {
        sol::VulkanCommandPoolPtr compute;
        sol::VulkanCommandPoolPtr graphics;
    } commandPools;
};
