#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-memory/fwd.h"
#include "sol-task/fwd.h"
#include "sol-window/fwd.h"

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

    uint32_t                     frameIndex = 0;
    uint32_t                     imageIndex = 0;
    sol::WindowPtr               window;
    sol::VulkanInstancePtr       instance;
    sol::VulkanSurfacePtr        surface;
    sol::VulkanPhysicalDevicePtr physicalDevice;
    sol::VulkanDevicePtr         device;
    sol::VulkanSwapchainPtr      swapchain;
    sol::MemoryManagerPtr        memoryManager;
    sol::TaskGraphPtr            taskGraph;

    struct
    {
        sol::VulkanCommandPoolPtr compute;
        sol::VulkanCommandPoolPtr graphics;
    } commandPools;
};
