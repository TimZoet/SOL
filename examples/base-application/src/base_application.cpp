#include "base-application/base_application.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <future>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "parsertongue/parser.h"
#include "sol-core/vulkan_bottom_level_acceleration_structure.h"
#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-core/vulkan_surface.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-memory/memory_manager.h"
#include "sol-task/task_graph.h"
#include "sol-window/window.h"

namespace
{
    std::vector              validationLayers = {std::string("VK_LAYER_KHRONOS_validation")};
    std::vector<std::string> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

}  // namespace

BaseApplication::BaseApplication() = default;

BaseApplication::~BaseApplication() noexcept = default;

bool BaseApplication::parse(const int argc, char** argv)
{
    auto       parser = pt::parser(argc, argv);
    const auto width  = parser.add_value<int32_t>('\0', "width");
    width->set_default(1024);
    const auto height = parser.add_value<int32_t>('\0', "height");
    height->set_default(512);
    const auto frames = parser.add_value<int32_t>('\0', "max_frames");
    frames->set_default(2);

    if (std::string e; !parser(e))
    {
        std::cerr << "Internal parsing error: " << e << std::endl;
        return false;
    }
    if (parser.display_help(std::cout)) return false;
    if (!parser.get_errors().empty())
    {
        parser.display_errors(std::cerr);
        return false;
    }

    args.width     = width->get_value();
    args.height    = height->get_value();
    args.maxFrames = frames->get_value();

    return true;
}

void BaseApplication::initialize()
{
    createWindow();
    createInstance();
    createSurface();
    createPhysicalDevice();
    createDevice();
    createSwapchain();
    createMemoryManager();
    createCommandPools();
}

void BaseApplication::run()
{
    std::atomic_bool done = false;

    while (!glfwWindowShouldClose(window->get()))
    {
        glfwPollEvents();

        done = false;
        taskGraph->start();

        auto future = std::async(std::launch::async, [&done, this] {
            while (!done)
            {
                auto task = taskGraph->getNext();
                if (task)
                    (*task)();
                else
                    std::this_thread::yield();
            }
        });

        taskGraph->end();
        done = true;
        future.wait();

        frameIndex.resource = (frameIndex.resource + 1) % args.maxFrames;
    }
    vkDeviceWaitIdle(device->get());
}

void BaseApplication::createWindow()
{
    window = std::make_unique<sol::Window>(
      std::array{static_cast<int32_t>(args.width), static_cast<int32_t>(args.height)}, "BaseApplication");
}

void BaseApplication::createInstance()
{
    sol::VulkanInstance::Settings instanceSettings;
    instanceSettings.applicationName = "BaseApplication";
    instanceSettings.extensions      = sol::Window::getRequiredExtensions();
    instanceSettings.extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instanceSettings.enableDebugging  = true;
    instanceSettings.enableValidation = true;
    // TODO: Do we need validation layers anymore when they are enabled for this application in the Vulkan Configurator?
    instanceSettings.validationLayers = validationLayers;
    instance                          = sol::VulkanInstance::create(instanceSettings);
}

void BaseApplication::createSurface()
{
    sol::VulkanSurface::Settings surfaceSettings;
    surfaceSettings.instance = instance;
    surfaceSettings.func     = [this](const sol::VulkanInstance& inst, VkSurfaceKHR* surf) {
        return glfwCreateWindowSurface(inst.get(), window->get(), nullptr, surf);
    };
    surface = sol::VulkanSurface::create(surfaceSettings);
}

void BaseApplication::createPhysicalDevice()
{
    createSupportedFeatures();

    for (const auto& e : getExtensions()) deviceExtensions.push_back(e);

    sol::VulkanPhysicalDevice::Settings physicalDeviceSettings;
    physicalDeviceSettings.instance = instance;
    physicalDeviceSettings.surface  = surface;
    physicalDeviceSettings.extensions.assign(deviceExtensions.begin(), deviceExtensions.end());
    physicalDeviceSettings.propertyFilter = [](const VkPhysicalDeviceProperties& props) {
        return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    };
    physicalDeviceSettings.features          = supportedFeatures.get();
    physicalDeviceSettings.featureFilter     = getFeatureFilter();
    physicalDeviceSettings.queueFamilyFilter = [](const std::vector<sol::VulkanQueueFamily>& queues) {
        bool hasGraphics          = false;
        bool hasPresent           = false;
        bool hasDedicatedTransfer = false;

        for (const auto& q : queues)
        {
            if (q.supportsGraphics()) hasGraphics = true;
            if (q.supportsPresent()) hasPresent = true;
            if (q.supportsDedicatedTransfer()) hasDedicatedTransfer = true;
        }

        return hasGraphics && hasPresent && hasDedicatedTransfer;
    };
    physicalDevice = sol::VulkanPhysicalDevice::create(physicalDeviceSettings);
}

void BaseApplication::createDevice()
{
    createEnabledFeatures();

    sol::VulkanDevice::Settings deviceSettings;
    deviceSettings.physicalDevice = physicalDevice;
    deviceSettings.extensions     = physicalDevice->getSettings().extensions;
    deviceSettings.features       = enabledFeatures.get();
    deviceSettings.queues.resize(physicalDevice->getQueueFamilies().size());
    std::ranges::fill(deviceSettings.queues.begin(), deviceSettings.queues.end(), 1);
    deviceSettings.threadSafeQueues = true;
    device                          = sol::VulkanDevice::create(deviceSettings);
}

void BaseApplication::createSwapchain()
{
    sol::VulkanSwapchain::Settings swapchainSettings;
    swapchainSettings.surface        = surface;
    swapchainSettings.physicalDevice = physicalDevice;
    swapchainSettings.device         = device;
    swapchainSettings.extent         = VkExtent2D{static_cast<uint32_t>(window->getFramebufferSize()[0]),
                                          static_cast<uint32_t>(window->getFramebufferSize()[1])};
    swapchain.resource               = sol::VulkanSwapchain::create(swapchainSettings);
}

void BaseApplication::createMemoryManager()
{
    sol::VulkanMemoryAllocator::Settings settings;
    settings.device = device;
    settings.flags  = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    memoryManager   = std::make_unique<sol::MemoryManager>(sol::VulkanMemoryAllocator::create(settings));
    // TODO: Handle situation where there is no queue for each family.
    for (auto& queue : device->getQueues())
    {
        if (queue->getFamily().supportsCompute()) memoryManager->setComputeQueue(*queue);
        if (queue->getFamily().supportsGraphics()) memoryManager->setGraphicsQueue(*queue);
        if (queue->getFamily().supportsDedicatedTransfer()) memoryManager->setTransferQueue(*queue);
    }

    computeQueue.resource  = &memoryManager->getComputeQueue();
    graphicsQueue.resource = &memoryManager->getGraphicsQueue();
}

void BaseApplication::createCommandPools()
{
    sol::VulkanCommandPool::Settings commandPoolSettings;
    commandPoolSettings.device           = device;
    commandPoolSettings.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolSettings.queueFamilyIndex = memoryManager->getComputeQueue().getFamily().getIndex();
    commandPools.compute                 = sol::VulkanCommandPool::create(commandPoolSettings);
    commandPoolSettings.queueFamilyIndex = memoryManager->getGraphicsQueue().getFamily().getIndex();
    commandPools.graphics                = sol::VulkanCommandPool::create(commandPoolSettings);
}
