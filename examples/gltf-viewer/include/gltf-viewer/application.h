#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-descriptor/fwd.h"
#include "sol-material/fwd.h"
#include "sol-memory/fwd.h"
#include "sol-mesh/fwd.h"
#include "sol-render/graphics/fwd.h"
#include "sol-window/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////


class Application
{
public:
    Application();

    Application(const Application&) = delete;

    Application(Application&&) noexcept = delete;

    ~Application() noexcept;

    Application& operator=(const Application&) = delete;

    Application& operator=(Application&&) noexcept = delete;

    [[nodiscard]] bool parse(int argc, char** argv);

    void initialize();

    void run();

    std::vector<std::string> getExtensions();

    void createSupportedFeatures();

    void createEnabledFeatures();

    std::function<bool(sol::RootVulkanPhysicalDeviceFeatures2&)> getFeatureFilter();

protected:
    void createWindow();

    void createInstance();

    void createSurface();

    void createPhysicalDevice();

    void createDevice();

    void createSwapchain();

    void createMemoryManager();

    void createCommandPools();

    void createSynchronization();

    void createRenderingInfo();

    void createCommandBuffers();

    void createGeometry();

    void createMaterials();

    struct
    {
        uint32_t width     = 0;
        uint32_t height    = 0;
        uint32_t maxFrames = 1;
    } args;

    sol::VulkanPhysicalDeviceFeatures2Ptr supportedFeatures;
    sol::VulkanPhysicalDeviceFeatures2Ptr enabledFeatures;

    sol::WindowPtr                             window;
    sol::VulkanInstancePtr                     instance;
    sol::VulkanSurfacePtr                      surface;
    sol::VulkanPhysicalDevicePtr               physicalDevice;
    sol::VulkanDevicePtr                       device;
    sol::VulkanSwapchainPtr                    swapchain;
    sol::MemoryManagerPtr                      memoryManager;
    sol::TransactionManagerPtr                 transactionManager;
    sol::VulkanSemaphorePtr                    swapchainSemaphore;
    sol::VulkanSemaphorePtr                    submitSemaphore;
    sol::VulkanFencePtr                        submitFence;
    std::vector<sol::GraphicsRenderingInfoPtr> renderingInfos;
    sol::VulkanCommandPoolPtr                  commandPool;
    std::vector<sol::VulkanCommandBufferPtr>   commandBuffers;
    sol::GeometryBufferAllocatorPtr            geometryAllocator;
    sol::VertexBufferPtr                       vertexBuffer;
    sol::GraphicsMaterial2Ptr                  material;
    std::vector<sol::DescriptorLayoutPtr>      descriptorLayouts;
};
