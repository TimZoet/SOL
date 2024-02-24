#include "gltf-viewer/application.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <fstream>
#include <future>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "parsertongue/parser.h"
#include "sol-core/utils.h"
#include "sol-core/vulkan_bottom_level_acceleration_structure.h"
#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_graphics_pipeline2.h"
#include "sol-core/vulkan_graphics_pipeline_fragment.h"
#include "sol-core/vulkan_graphics_pipeline_fragment_output.h"
#include "sol-core/vulkan_graphics_pipeline_pre_rasterization.h"
#include "sol-core/vulkan_graphics_pipeline_vertex_input.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_memory_allocator.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_pipeline_layout.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_queue_family.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_shader_module.h"
#include "sol-core/vulkan_surface.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-descriptor/fwd.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-error/vulkan_no_devices_error.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transaction_manager.h"
#include "sol-mesh/geometry_buffer_allocator.h"
#include "sol-mesh/vertex_buffer.h"
#include "sol-render/graphics/graphics_rendering_info.h"
#include "sol-window/window.h"

namespace
{
    std::vector<std::string> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    [[nodiscard]] std::vector<std::byte> loadShaderBytecode(const std::filesystem::path& filename)
    {
        std::ifstream          file(filename, std::ios::binary | std::ios::ate);
        const auto             size = file.tellg();
        std::vector<std::byte> code(size);
        file.seekg(std::ios::beg);
        file.read(reinterpret_cast<char*>(code.data()), size);
        return code;
    }
}  // namespace

Application::Application() = default;

Application::~Application() noexcept = default;

bool Application::parse(const int argc, char** argv)
{
    auto       parser = pt::parser(argc, argv);
    const auto width  = parser.add_value<int32_t>('\0', "width");
    width->set_default(1024);
    const auto height = parser.add_value<int32_t>('\0', "height");
    height->set_default(512);

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

    args.width  = width->get_value();
    args.height = height->get_value();

    return true;
}

void Application::initialize()
{

    createWindow();
    createInstance();
    createSurface();
    createPhysicalDevice();
    createDevice();
    createSwapchain();
    createMemoryManager();
    createCommandPools();
    createSynchronization();
    createRenderingInfo();
    createCommandBuffers();
    createGeometry();
    createMaterials();
}

void Application::run()
{
    uint32_t imageIndex = 0;
    uint32_t frameIndex = 0;

    while (!glfwWindowShouldClose(window->get()))
    {
        glfwPollEvents();

        // Wait for submit fence.
        vkWaitForFences(device->get(), 1, &submitFence->get(), VK_TRUE, UINT64_MAX);
        vkResetFences(device->get(), 1, &submitFence->get());

        // TODO: Traverse.

        // Acquire image from swapchain.
        {
            const auto result = vkAcquireNextImageKHR(device->get(),
                                                      swapchain->get(),
                                                      std::numeric_limits<uint64_t>::max(),
                                                      swapchainSemaphore->get(),
                                                      VK_NULL_HANDLE,
                                                      &imageIndex);

            // TODO: Add member variable with function to recreate swapchain.
            if (result == VK_ERROR_OUT_OF_DATE_KHR)  // || result == VK_SUBOPTIMAL_KHR
            {
                /*vkDeviceWaitIdle(taskGraph->getDevice().get());
                (*swapchain)->recreate();
                if (recreateFunction) recreateFunction(**swapchain);*/
            }
            //if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) handleVulkanError(result);
        }

        // TODO: Render.
        {
            commandBuffers[frameIndex]->resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            commandBuffers[frameIndex]->beginOneTimeCommand();
            renderingInfos[imageIndex]->beginRendering(*commandBuffers[frameIndex]);

            vkCmdBindPipeline(
              commandBuffers[frameIndex]->get(), VK_PIPELINE_BIND_POINT_GRAPHICS, material->getPipeline().get());
            const VkViewport viewport{.x        = 0,
                                      .y        = 0,
                                      .width    = static_cast<float>(swapchain->getExtent().width),
                                      .height   = static_cast<float>(swapchain->getExtent().height),
                                      .minDepth = 0,
                                      .maxDepth = 1};
            vkCmdSetViewportWithCount(commandBuffers[frameIndex]->get(), 1, &viewport);
            const VkRect2D scissor{.offset = VkOffset2D{.x = 0, .y = 0}, .extent = swapchain->getExtent()};
            vkCmdSetScissorWithCount(commandBuffers[frameIndex]->get(), 1, &scissor);

            const auto offset = vertexBuffer->getBufferOffset();
            vkCmdBindVertexBuffers(commandBuffers[frameIndex]->get(), 0, 1, &vertexBuffer->getBuffer().get(), &offset);
            vkCmdDraw(commandBuffers[frameIndex]->get(), 3, 1, 0, 0);

            renderingInfos[imageIndex]->endRendering(*commandBuffers[frameIndex]);
            commandBuffers[frameIndex]->endCommand();
        }

        // Submit.
        {
            const std::array<VkPipelineStageFlags, 1> flags = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

            VkSubmitInfo submitInfo{};
            submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = &swapchainSemaphore->get();
            submitInfo.pWaitDstStageMask    = flags.data();
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &commandBuffers[frameIndex]->get();
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &submitSemaphore->get();

            sol::handleVulkanError(
              vkQueueSubmit(memoryManager->getGraphicsQueue().get(), 1, &submitInfo, submitFence->get()));
        }

        // Present image.
        {
            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            // TODO: Wait for submit to finish.
            presentInfo.waitSemaphoreCount = 1;  // static_cast<uint32_t>(waitSemaphoreHandles.size());
            presentInfo.pWaitSemaphores    = &submitSemaphore->get();
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &swapchain->get();
            presentInfo.pImageIndices      = &imageIndex;


            // TODO: Handle swapchain changes.
            const auto result = vkQueuePresentKHR(memoryManager->getGraphicsQueue().get(), &presentInfo);
            if (result == VK_ERROR_OUT_OF_DATE_KHR)  // || result == VK_SUBOPTIMAL_KHR || framebufferResized
            {
                /*vkDeviceWaitIdle(getDevice().get());
                rSwapchain.recreate();
                if (recreateFunction) recreateFunction(rSwapchain);*/
            }
            else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { sol::handleVulkanError(result); }
        }


        // Increment frame.
        frameIndex = (frameIndex + 1) % args.maxFrames;
    }

    vkDeviceWaitIdle(device->get());
}

std::vector<std::string> Application::getExtensions() { return {VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME}; }

void Application::createSupportedFeatures()
{
    supportedFeatures =
      std::make_unique<sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                                          sol::VulkanPhysicalDeviceVulkan12Features,
                                                          sol::VulkanPhysicalDeviceVulkan13Features,
                                                          sol::VulkanPhysicalDeviceMaintenance5FeaturesKHR,
                                                          sol::VulkanPhysicalDeviceDescriptorBufferFeaturesEXT>>();
}

void Application::createEnabledFeatures()
{
    enabledFeatures =
      std::make_unique<sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                                          sol::VulkanPhysicalDeviceVulkan12Features,
                                                          sol::VulkanPhysicalDeviceVulkan13Features,
                                                          sol::VulkanPhysicalDeviceMaintenance5FeaturesKHR,
                                                          sol::VulkanPhysicalDeviceDescriptorBufferFeaturesEXT>>();

    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress         = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing          = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering            = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->timelineSemaphore           = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2            = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceMaintenance5FeaturesKHR>()->maintenance5         = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceDescriptorBufferFeaturesEXT>()->descriptorBuffer = VK_TRUE;
}

std::function<bool(sol::RootVulkanPhysicalDeviceFeatures2&)> Application::getFeatureFilter()
{
    return [](sol::RootVulkanPhysicalDeviceFeatures2& features) {
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2) return false;

        return true;
    };
}

void Application::createWindow()
{
    window = std::make_unique<sol::Window>(
      std::array{static_cast<int32_t>(args.width), static_cast<int32_t>(args.height)}, "Application");
}

void Application::createInstance()
{
    sol::VulkanInstance::Settings instanceSettings;
    instanceSettings.applicationName = "Application";
    instanceSettings.extensions      = sol::Window::getRequiredExtensions();
    instanceSettings.extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instanceSettings.enableDebugging = true;
    instance                         = sol::VulkanInstance::create(instanceSettings);
}

void Application::createSurface()
{
    sol::VulkanSurface::Settings surfaceSettings;
    surfaceSettings.instance = instance;
    surfaceSettings.func     = [this](const sol::VulkanInstance& inst, VkSurfaceKHR* surf) {
        return glfwCreateWindowSurface(inst.get(), window->get(), nullptr, surf);
    };
    surface = sol::VulkanSurface::create(surfaceSettings);
}

void Application::createPhysicalDevice()
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

void Application::createDevice()
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

void Application::createSwapchain()
{
    sol::VulkanSwapchain::Settings swapchainSettings;
    swapchainSettings.surface        = surface;
    swapchainSettings.physicalDevice = physicalDevice;
    swapchainSettings.device         = device;
    swapchainSettings.extent         = VkExtent2D{static_cast<uint32_t>(window->getFramebufferSize()[0]),
                                          static_cast<uint32_t>(window->getFramebufferSize()[1])};
    swapchain                        = sol::VulkanSwapchain::create(swapchainSettings);
}

void Application::createMemoryManager()
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

    transactionManager = sol::TransactionManager::create(*memoryManager, 256ull * 1024ull * 1024ull);
}

void Application::createCommandPools()
{
    sol::VulkanCommandPool::Settings commandPoolSettings;
    commandPoolSettings.device           = device;
    commandPoolSettings.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolSettings.queueFamilyIndex = memoryManager->getGraphicsQueue().getFamily().getIndex();
    commandPool                          = sol::VulkanCommandPool::create(commandPoolSettings);
}

void Application::createSynchronization()
{
    {
        sol::VulkanSemaphore::Settings settings;
        settings.device = device;

        swapchainSemaphore = sol::VulkanSemaphore::create(settings);
        submitSemaphore    = sol::VulkanSemaphore::create(settings);
    }
    {
        sol::VulkanFence::Settings settings;
        settings.device   = device;
        settings.signaled = true;
        submitFence       = sol::VulkanFence::create(settings);
    }
}

void Application::createRenderingInfo()
{
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
        renderingInfos.emplace_back(std::move(info));
    }
}

void Application::createCommandBuffers()
{
    sol::VulkanCommandBuffer::Settings settings;
    settings.commandPool = commandPool;
    commandBuffers       = sol::VulkanCommandBuffer::create(settings, args.maxFrames);
}

void Application::createGeometry()
{
    const sol::GeometryBufferAllocator::Settings settings{.memoryManager = *memoryManager,
                                                          .strategy = sol::GeometryBufferAllocator::Strategy::Separate};
    geometryAllocator = sol::GeometryBufferAllocator::create(settings);

    vertexBuffer                 = geometryAllocator->allocateVertexBuffer(6, sizeof(math::float2));
    const auto       transaction = transactionManager->beginTransaction();
    const std::array vertices    = {math::float2{-1.0f, -1.0f},
                                    math::float2{1.0f, -1.0f},
                                    math::float2{1.0f, 1.0f},
                                    math::float2{1.0f, 1.0f},
                                    math::float2{-1.0f, 1.0f},
                                    math::float2{-1.0f, 0.0f}};
    const auto       success =
      vertexBuffer->setVertexData(*transaction,
                                  vertices.data(),
                                  6,
                                  0,
                                  sol::IBuffer::Barrier{.dstFamily = &memoryManager->getGraphicsQueue().getFamily(),
                                                        // There were no previous commands yet.
                                                        .srcStage = VK_PIPELINE_STAGE_2_NONE,
                                                        // Buffer is going to be used as input for draw commands.
                                                        .dstStage = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
                                                        // There were no previous commands yet.
                                                        .srcAccess = VK_ACCESS_2_NONE,
                                                        // Buffer is going to be used as input for draw commands.
                                                        .dstAccess = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT},
                                  false);
    if (!success) throw std::runtime_error("Staging buffer allocation failed.");
    transaction->commit();
    transaction->wait();
}

void Application::createMaterials()
{


    descriptorLayouts.emplace_back(std::make_unique<sol::DescriptorLayout>(*device));
    // descriptorLayouts.back()->add(sol::DescriptorLayout::UniformBufferBinding{
    //  .binding = 0, .size = sizeof(float) * 16, .count = 1, .stages = VK_SHADER_STAGE_VERTEX_BIT});
    descriptorLayouts.back()->finalize();

    /*descriptorLayouts.emplace_back(std::make_unique<sol::DescriptorLayout>(*device));
    descriptorLayouts.back()->add(
      sol::DescriptorLayout::SampledImageBinding{.binding = 0, .count = 1, .stages = VK_SHADER_STAGE_FRAGMENT_BIT});
    descriptorLayouts.back()->add(
      sol::DescriptorLayout::SamplerBinding{.binding = 1, .count = 1, .stages = VK_SHADER_STAGE_FRAGMENT_BIT});
    descriptorLayouts.back()->finalize();*/

    sol::VulkanPipelineLayout::Settings layoutSettings;
    layoutSettings.device = device;
    //layoutSettings.descriptors += descriptorLayouts[0]->getLayout();
    //layoutSettings.descriptors += descriptorLayouts[1]->getLayout();
    auto layout = sol::VulkanPipelineLayout::create(layoutSettings);

    sol::VulkanGraphicsPipelineVertexInput::Settings vertexInputSettings;
    vertexInputSettings.device = device;
    vertexInputSettings.vertexAttributes.emplace_back(0, 0, VK_FORMAT_R32G32_SFLOAT, 0);
    //vertexInputSettings.vertexAttributes.emplace_back(1, 0, VK_FORMAT_R32G32_SFLOAT, 12);
    vertexInputSettings.vertexBindings.emplace_back(0, 8, VK_VERTEX_INPUT_RATE_VERTEX);


    sol::VulkanGraphicsPipelinePreRasterization::Settings preRastSettings;
    preRastSettings.layout = layout;
    preRastSettings.vertexShader.code =
      loadShaderBytecode("C:/Users/timzo/dev/projects/sol/source/examples/gltf-viewer/shaders/display.vert.spv");
    preRastSettings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT);
    preRastSettings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT);
    preRastSettings.rasterization.cullMode = VK_CULL_MODE_NONE;


    sol::VulkanGraphicsPipelineFragment::Settings fragmentSettings;
    fragmentSettings.layout = layout;
    fragmentSettings.fragmentShader.code =
      loadShaderBytecode("C:/Users/timzo/dev/projects/sol/source/examples/gltf-viewer/shaders/display.frag.spv");


    sol::VulkanGraphicsPipelineFragmentOutput::Settings fragOutSettings;
    fragOutSettings.device = device;
    fragOutSettings.colorBlend.attachments.emplace_back(VK_FALSE);
    fragOutSettings.colorAttachmentFormats.push_back(swapchain->getImageViews()[0]->getSettings().format);
    fragOutSettings.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

    // graphics_pipeline_library is not enabled by default, so creating complete pipeline for now.
#if 0
    auto preRastPipeline = sol::VulkanGraphicsPipelinePreRasterization::create(preRastSettings);
    auto vertexInputPipeline = sol::VulkanGraphicsPipelineVertexInput::create(vertexInputSettings);
    auto fragPipeline = sol::VulkanGraphicsPipelineFragment::create(fragmentSettings);
    auto fragOutPipeline = sol::VulkanGraphicsPipelineFragmentOutput::create(fragOutSettings);

    sol::VulkanGraphicsPipeline2::Settings pipelineSettings;
    pipelineSettings.device = device;
    pipelineSettings.vertexInputPipeline = vertexInputPipeline;
    pipelineSettings.preRasterizationPipeline = preRastPipeline;
    pipelineSettings.fragmentPipeline = fragPipeline;
    pipelineSettings.fragmentOutputPipeline = fragOutPipeline;
    auto pipeline = sol::VulkanGraphicsPipeline2::create(pipelineSettings);
#else
    sol::VulkanGraphicsPipeline2::Settings2 pipelineSettings;
    pipelineSettings.vertexInput      = vertexInputSettings;
    pipelineSettings.preRasterization = preRastSettings;
    pipelineSettings.fragment         = fragmentSettings;
    pipelineSettings.fragmentOutput   = fragOutSettings;
    auto pipeline                     = sol::VulkanGraphicsPipeline2::create2(pipelineSettings);
#endif

    material = std::make_unique<sol::GraphicsMaterial2>(std::move(pipeline), raw(descriptorLayouts));
}
