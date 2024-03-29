#include "testutils/utils.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <fstream>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_graphics_pipeline2.h"
#include "sol-core/vulkan_graphics_pipeline_fragment.h"
#include "sol-core/vulkan_graphics_pipeline_fragment_output.h"
#include "sol-core/vulkan_graphics_pipeline_pre_rasterization.h"
#include "sol-core/vulkan_graphics_pipeline_vertex_input.h"
#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_pipeline_layout.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_surface.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transaction_manager.h"
#include "sol-mesh/mesh_layout.h"
#if 0
#include "sol-render/graphics/fwd.h"
#include "sol-render/graphics/graphics_rendering_info.h"
#endif
#include "sol-window/fwd.h"
#include "sol-window/window.h"

namespace
{
    sol::WindowPtr                        window;
    sol::VulkanInstancePtr                instance;
    sol::VulkanPhysicalDeviceFeatures2Ptr supportedFeatures;
    sol::VulkanPhysicalDeviceFeatures2Ptr enabledFeatures;
    sol::VulkanSurfacePtr                 surface;
    sol::VulkanPhysicalDevicePtr          physicalDevice;
    sol::VulkanDevicePtr                  device;
    sol::VulkanSwapchainPtr               swapchain;
    sol::VulkanCommandBufferPtr           swapchainCommandBuffer;
    sol::VulkanFencePtr                   acquireFence;
    sol::VulkanFencePtr                   presentFence;
    uint32_t                              imageIndex = 0;
#if 0
    std::vector<sol::GraphicsRenderingInfoPtr> renderingInfos;
#endif
    sol::MemoryManagerPtr      memoryManager;
    sol::TransactionManagerPtr transferManager;

    void createDefaultWindow() { window = std::make_unique<sol::Window>(std::array{1024, 512}, "Test"); }

    void createDefaultInstance()
    {
        sol::VulkanInstance::Settings settings;
        settings.applicationName    = "SolTest";
        settings.applicationVersion = sol::Version(1, 0, 0);
        settings.extensions         = sol::Window::getRequiredExtensions();
        instance                    = sol::VulkanInstance::create(settings);
    }

    void createSupportedFeatures()
    {
        supportedFeatures = std::make_unique<
          sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                             sol::VulkanPhysicalDeviceVulkan12Features,
                                             sol::VulkanPhysicalDeviceVulkan13Features,
                                             sol::VulkanPhysicalDeviceMaintenance5FeaturesKHR,
                                             sol::VulkanPhysicalDeviceDescriptorBufferFeaturesEXT,
                                             sol::VulkanPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT>>();
    }

    void createEnabledFeatures()
    {
        enabledFeatures = std::make_unique<
          sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                             sol::VulkanPhysicalDeviceVulkan12Features,
                                             sol::VulkanPhysicalDeviceVulkan13Features,
                                             sol::VulkanPhysicalDeviceMaintenance5FeaturesKHR,
                                             sol::VulkanPhysicalDeviceDescriptorBufferFeaturesEXT,
                                             sol::VulkanPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT>>();

        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress         = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing          = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->timelineSemaphore           = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering            = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2            = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceMaintenance5FeaturesKHR>()->maintenance5         = VK_TRUE;
        enabledFeatures->getAs<sol::VulkanPhysicalDeviceDescriptorBufferFeaturesEXT>()->descriptorBuffer = VK_TRUE;
        /* enabledFeatures->getAs<sol::VulkanPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT>()->graphicsPipelineLibrary =
          VK_TRUE;*/
    }

    void createDefaultSurface()
    {
        sol::VulkanSurface::Settings settings;
        settings.instance = instance;
        settings.func     = [&](const sol::VulkanInstance& inst, VkSurfaceKHR* surf) {
            return glfwCreateWindowSurface(inst.get(), window->get(), nullptr, surf);
        };
        surface = sol::VulkanSurface::create(settings);
    }

    void createDefaultPhysicalDevice(const bool enableFrame)
    {
        sol::VulkanPhysicalDevice::Settings settings;
        settings.instance = instance;
        settings.surface  = surface;
        settings.extensions.emplace_back(VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME);
        settings.extensions.emplace_back(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
        //settings.extensions.emplace_back(VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME);
        if (enableFrame) settings.extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        settings.propertyFilter = [](const VkPhysicalDeviceProperties& props) {
            return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        };
        // TODO: Many of these features are kinda needed by SOL out of the box. Perhaps something needs to be built in to SOL to improve this.
        settings.features      = supportedFeatures.get();
        settings.featureFilter = [](sol::RootVulkanPhysicalDeviceFeatures2& features) {
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->timelineSemaphore) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceMaintenance5FeaturesKHR>()->maintenance5) return false;
            if (!features.getAs<sol::VulkanPhysicalDeviceDescriptorBufferFeaturesEXT>()->descriptorBuffer) return false;
            // Cannot enable this on AMD Windows. Ugh...
            /*if (!features.getAs<sol::VulkanPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT>()->graphicsPipelineLibrary)
                return false;*/
            return true;
        };
        settings.queueFamilyFilter = [](const std::vector<sol::VulkanQueueFamily>& queues) {
            bool hasGraphics          = false;
            bool hasDedicatedTransfer = false;

            for (const auto& q : queues)
            {
                if (q.supportsGraphics()) hasGraphics = true;
                if (q.supportsDedicatedTransfer()) hasDedicatedTransfer = true;
            }

            return hasGraphics && hasDedicatedTransfer;
        };
        physicalDevice = sol::VulkanPhysicalDevice::create(settings);
    }

    void createDefaultDevice()
    {
        sol::VulkanDevice::Settings settings;
        settings.physicalDevice = physicalDevice;
        settings.extensions     = physicalDevice->getSettings().extensions;
        settings.features       = enabledFeatures.get();
        settings.queues.resize(physicalDevice->getQueueFamilies().size(), 1);
        settings.threadSafeQueues = true;
        device                    = sol::VulkanDevice::create(settings);
    }

    void createDefaultSwapchain()
    {
        sol::VulkanSwapchain::Settings settings;
        settings.surface        = surface;
        settings.physicalDevice = physicalDevice;
        settings.device         = device;
        settings.extent         = VkExtent2D{static_cast<uint32_t>(window->getFramebufferSize()[0]),
                                     static_cast<uint32_t>(window->getFramebufferSize()[1])};
        swapchain               = sol::VulkanSwapchain::create(settings);
    }

    void createDefaultSwapchainCommandBuffer()
    {
        sol::VulkanCommandBuffer::Settings settings;
        settings.commandPool   = memoryManager->getCommandPool(memoryManager->getGraphicsQueue().getFamily());
        settings.level         = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        swapchainCommandBuffer = sol::VulkanCommandBuffer::create(settings);
    }

    void createRenderingInfos()
    {
#if 0
        for (size_t i = 0; i < swapchain->getImageCount(); i++)
        {
            auto info = std::make_unique<sol::GraphicsRenderingInfo>();
            info->setRenderArea(0, 0, swapchain->getExtent().width, swapchain->getExtent().height);
            info->setLayerCount(1);
            info->addColorAttachment(*swapchain->getImageViews()[i],
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     VK_ATTACHMENT_STORE_OP_STORE,
                                     std::array<uint32_t, 4>{0, 0, 0, 0});
            info->setColorAttachmentPreTransition(0,
                                                  nullptr,
                                                  nullptr,
                                                  VK_IMAGE_LAYOUT_UNDEFINED,
                                                  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                  VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                                  VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                  0,
                                                  VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
            info->setColorAttachmentPostTransition(0,
                                                   nullptr,
                                                   nullptr,
                                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                   VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                   VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
                                                   VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                                   0);
            info->finalize();
            renderingInfos.emplace_back(std::move(info));
        }
#endif
    }

    void createPresentFence()
    {
        sol::VulkanFence::Settings settings;
        settings.device = device;
        acquireFence    = sol::VulkanFence::create(settings);
        presentFence    = sol::VulkanFence::create(settings);
    }

    void createDefaultMemoryManager()
    {
        sol::VulkanMemoryAllocator::Settings settings;
        settings.device = *device;
        settings.flags  = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        memoryManager   = std::make_unique<sol::MemoryManager>(sol::VulkanMemoryAllocator::create(settings));
        for (auto& queue : device->getQueues())
        {
            if (queue->getFamily().supportsCompute()) memoryManager->setComputeQueue(*queue);
            if (queue->getFamily().supportsGraphics()) memoryManager->setGraphicsQueue(*queue);
            if (queue->getFamily().supportsDedicatedTransfer()) memoryManager->setTransferQueue(*queue);
        }
    }

    void createDefaultTransferManager()
    {
        constexpr sol::IMemoryPool::CreateInfo info{
          .createFlags          = 0,
          .bufferUsage          = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
          .memoryUsage          = VMA_MEMORY_USAGE_AUTO,
          .requiredMemoryFlags  = 0,
          .preferredMemoryFlags = 0,
          .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
          .blockSize       = 256ull * 1024ull * 1024ull,
          .minBlocks       = 1,
          .maxBlocks       = 1};
        auto& pool      = memoryManager->createRingBufferMemoryPool("transfer", info);
        transferManager = std::make_unique<sol::TransactionManager>(*memoryManager, pool);
    }

    
}  // namespace


BasicFixture::BasicFixture(const bool enableFrame)
{
    if (enableFrame) createDefaultWindow();
    createDefaultInstance();
    createSupportedFeatures();
    createEnabledFeatures();
    if (enableFrame) createDefaultSurface();
    createDefaultPhysicalDevice(enableFrame);
    createDefaultDevice();
    createDefaultMemoryManager();
    if (enableFrame)
    {
        createDefaultSwapchain();
        createDefaultSwapchainCommandBuffer();
        createPresentFence();
        createRenderingInfos();
    }
    createDefaultTransferManager();
}

BasicFixture::~BasicFixture() noexcept
{
    vkDeviceWaitIdle(device->get());
    transferManager.reset();
#if 0
    renderingInfos.clear();
#endif
    presentFence.reset();
    acquireFence.reset();
    swapchainCommandBuffer.reset();
    swapchain.reset();
    memoryManager.reset();
    device.reset();
    physicalDevice.reset();
    surface.reset();
    enabledFeatures.reset();
    supportedFeatures.reset();
    instance.reset();
    window.reset();
}

sol::VulkanInstance& BasicFixture::getInstance() { return *instance; }

sol::VulkanPhysicalDevice& BasicFixture::getPhysicalDevice() { return *physicalDevice; }

sol::VulkanDevice& BasicFixture::getDevice() { return *device; }

sol::MemoryManager& BasicFixture::getMemoryManager() { return *memoryManager; }

sol::TransactionManager& BasicFixture::getTransferManager() { return *transferManager; }

void BasicFixture::acquire()
{
    sol::handleVulkanError(vkAcquireNextImageKHR(device->get(),
                                                 swapchain->get(),
                                                 std::numeric_limits<uint64_t>::max(),
                                                 VK_NULL_HANDLE,
                                                 acquireFence->get(),
                                                 &imageIndex));
}

void BasicFixture::render()
{
#if 0
    const VkFence f = acquireFence->get();
    vkWaitForFences(device->get(), 1, &f, true, UINT64_MAX);
    vkResetFences(device->get(), 1, &f);
    swapchainCommandBuffer->resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    swapchainCommandBuffer->beginOneTimeCommand();
    renderingInfos[imageIndex]->preTransition(*swapchainCommandBuffer);
    renderingInfos[imageIndex]->beginRendering(*swapchainCommandBuffer);
    renderingInfos[imageIndex]->endRendering(*swapchainCommandBuffer);
    renderingInfos[imageIndex]->postTransition(*swapchainCommandBuffer);
    swapchainCommandBuffer->endCommand();

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &swapchainCommandBuffer->get();

    sol::handleVulkanError(vkQueueSubmit(memoryManager->getGraphicsQueue().get(), 1, &submitInfo, presentFence->get()));
#endif
}

void BasicFixture::present()
{
    const VkFence f = presentFence->get();
    vkWaitForFences(device->get(), 1, &f, true, UINT64_MAX);
    vkResetFences(device->get(), 1, &f);

    VkPresentInfoKHR     presentInfo{};
    const VkSwapchainKHR swapchains[] = {swapchain->get()};
    presentInfo.sType                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount    = 0;
    presentInfo.pWaitSemaphores       = nullptr;
    presentInfo.swapchainCount        = 1;
    presentInfo.pSwapchains           = swapchains;
    presentInfo.pImageIndices         = &imageIndex;
    vkQueuePresentKHR(memoryManager->getGraphicsQueue().get(), &presentInfo);
}

void BasicFixture::frame()
{
    acquire();
    render();
    present();
}

#if 0
std::pair<sol::VulkanGraphicsPipeline2Ptr, std::vector<sol::DescriptorLayoutPtr>>
  BasicFixture::createSimpleGraphicsPipeline() const
{
    std::vector<sol::DescriptorLayoutPtr> descriptorLayouts;

    descriptorLayouts.emplace_back(std::make_unique<sol::DescriptorLayout>(getDevice()));
    descriptorLayouts.back()->add(sol::DescriptorLayout::UniformBufferBinding{
      .binding = 0, .size = sizeof(float) * 16, .count = 1, .stages = VK_SHADER_STAGE_VERTEX_BIT});
    descriptorLayouts.back()->finalize();

    descriptorLayouts.emplace_back(std::make_unique<sol::DescriptorLayout>(getDevice()));
    descriptorLayouts.back()->add(
      sol::DescriptorLayout::SampledImageBinding{.binding = 0, .count = 1, .stages = VK_SHADER_STAGE_FRAGMENT_BIT});
    descriptorLayouts.back()->add(
      sol::DescriptorLayout::SamplerBinding{.binding = 1, .count = 1, .stages = VK_SHADER_STAGE_FRAGMENT_BIT});
    descriptorLayouts.back()->finalize();

    sol::VulkanPipelineLayout::Settings layoutSettings;
    layoutSettings.device = getDevice();
    layoutSettings.descriptors += descriptorLayouts[0]->getLayout();
    layoutSettings.descriptors += descriptorLayouts[1]->getLayout();
    auto layout = sol::VulkanPipelineLayout::create(layoutSettings);

    sol::VulkanGraphicsPipelineVertexInput::Settings vertexInputSettings;
    vertexInputSettings.device = getDevice();
    vertexInputSettings.vertexAttributes.emplace_back(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
    vertexInputSettings.vertexAttributes.emplace_back(1, 0, VK_FORMAT_R32G32_SFLOAT, 12);
    vertexInputSettings.vertexBindings.emplace_back(0, 20, VK_VERTEX_INPUT_RATE_VERTEX);


    sol::VulkanGraphicsPipelinePreRasterization::Settings preRastSettings;
    preRastSettings.layout            = layout;
    preRastSettings.vertexShader.code = loadShaderBytecode("simple_vs.spv");
    preRastSettings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT);
    preRastSettings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT);


    sol::VulkanGraphicsPipelineFragment::Settings fragmentSettings;
    fragmentSettings.layout              = layout;
    fragmentSettings.fragmentShader.code = loadShaderBytecode("simple_ps.spv");


    sol::VulkanGraphicsPipelineFragmentOutput::Settings fragOutSettings;
    fragOutSettings.device = getDevice();
    fragOutSettings.colorBlend.attachments.emplace_back(VK_FALSE);
    fragOutSettings.colorAttachmentFormats.push_back(VK_FORMAT_R8G8B8A8_SRGB);
    fragOutSettings.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

    // graphics_pipeline_library is not enabled by default, so creating complete pipeline for now.
#if 0
    auto preRastPipeline = sol::VulkanGraphicsPipelinePreRasterization::create(preRastSettings);
    auto vertexInputPipeline = sol::VulkanGraphicsPipelineVertexInput::create(vertexInputSettings);
    auto fragPipeline = sol::VulkanGraphicsPipelineFragment::create(fragmentSettings);
    auto fragOutPipeline = sol::VulkanGraphicsPipelineFragmentOutput::create(fragOutSettings);

    sol::VulkanGraphicsPipeline2::Settings pipelineSettings;
    pipelineSettings.device                   = getDevice();
    pipelineSettings.vertexInputPipeline      = vertexInputPipeline;
    pipelineSettings.preRasterizationPipeline = preRastPipeline;
    pipelineSettings.fragmentPipeline         = fragPipeline;
    pipelineSettings.fragmentOutputPipeline   = fragOutPipeline;
    auto pipeline                             = sol::VulkanGraphicsPipeline2::create(pipelineSettings);
#else
    sol::VulkanGraphicsPipeline2::Settings2 pipelineSettings;
    pipelineSettings.vertexInput      = vertexInputSettings;
    pipelineSettings.preRasterization = preRastSettings;
    pipelineSettings.fragment         = fragmentSettings;
    pipelineSettings.fragmentOutput   = fragOutSettings;
    auto pipeline                     = sol::VulkanGraphicsPipeline2::create2(pipelineSettings);
#endif

    return {std::move(pipeline), std::move(descriptorLayouts)};
}
#endif

std::vector<uint32_t> ImageDataGeneration::genR8G8B8A8W256H256Gradient()
{
    std::vector<uint32_t> data(256ull * 256ull, 0);
    for (const auto y : std::views::iota(0) | std::views::take(256))
        for (const auto x : std::views::iota(0) | std::views::take(256))
            data[x + y * 256] = x << 24 | y << 16 | std::max(x, y) << 8 | 0xff;

    return data;
}
