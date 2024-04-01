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
#include "sol-scenegraph/fwd.h"
#include "sol-task/compiled_graph.h"
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

    void createRenderingInfo();

    void createGeometry();

    void createMaterials();

    void createScenegraph();

    void createTaskGraph();

    struct
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t maxFrames = 2;
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
    std::vector<sol::GraphicsRenderingInfoPtr> renderingInfos;
    sol::VulkanCommandPoolPtr                  commandPool;
    sol::GeometryBufferAllocatorPtr            geometryAllocator;
    sol::VertexBufferPtr                       vertexBuffer;
    sol::MeshPtr                               mesh;
    sol::DescriptorBufferPtr                   descriptorBuffer;
    sol::GraphicsMaterial2Ptr                  material;
    sol::GraphicsMaterialInstance2Ptr          materialInstance;
    std::vector<sol::DescriptorLayoutPtr>      descriptorLayouts;
    sol::ScenegraphPtr                         scenegraph;
    sol::CompiledGraphPtr                      taskGraph;
};
