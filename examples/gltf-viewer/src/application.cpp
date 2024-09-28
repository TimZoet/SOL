#include "gltf-viewer/application.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <chrono>
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
#include "sol-core/vulkan_surface.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-descriptor/descriptor.h"
#include "sol-descriptor/descriptor_buffer.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-memory/memory_manager.h"
#include "sol-memory/transaction_manager.h"
#include "sol-mesh/geometry_buffer_allocator.h"
#include "sol-mesh/mesh.h"
#include "sol-mesh/vertex_buffer.h"
#include "sol-render/graphics/graphics_renderer.h"
#include "sol-render/graphics/graphics_rendering_info.h"
#include "sol-render/graphics/graphics_render_data.h"
#include "sol-render/graphics/graphics_traverser.h"
#include "sol-scenegraph/scenegraph.h"
#include "sol-scenegraph/drawable/mesh_node.h"
#include "sol-scenegraph/graphics/graphics_dynamic_state_node.h"
#include "sol-scenegraph/graphics/graphics_material_node.h"
#include "sol-task/task_graph.h"
#include "sol-task/resources/command_buffer_resource.h"
#include "sol-task/resources/index_resource.h"
#include "sol-task/tasks/acquire_task.h"
#include "sol-task/tasks/custom_task.h"
#include "sol-task/tasks/present_task.h"
#include "sol-task/tasks/render_task.h"
#include "sol-task/tasks/submit_task.h"
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
    createRenderingInfo();
    createGeometry();
    createMaterials();
    createScenegraph();
    createTaskGraph();
}

void Application::run()
{
    const auto start = std::chrono::high_resolution_clock::now();

    const auto lookAt = transpose(look_at(math::float3{ 10.0f, 10.0f, 10.0f }, math::float3{ 0.0f }));
    modelMatrixBuffer->getBuffer().setData(&lookAt, sizeof lookAt);

    while (!glfwWindowShouldClose(window->get()))
    {
        const auto time = std::chrono::high_resolution_clock::now();
        auto diff = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(time - start).count() % 1000) * 0.001f;
        const auto model = transpose(translation(math::float3{ 0,0.0f,0.0f }));// transpose(math::rotation_z(diff * math::m_pi * 2.0f));
        cameraMatrixBuffer->getBuffer().setData(&model, sizeof model);

        /*diff = 0;
        const auto lookAt = transpose(look_at(math::float3{ 10.0f * math::cos(diff * math::m_pi * 2), 10.0f, 10.0f * math::sin(diff * math::m_pi * 2)}, math::float3{0.0f}));
        cameraMatrixBuffer->getBuffer().setData(&lookAt, sizeof lookAt);*/

        glfwPollEvents();

        std::atomic_bool done = false;
        taskGraph->start();
        const auto runner = [&done, this] {
            while (!done)
            {
                auto task = taskGraph->getNext();
                if (task)
                    task->operator()();
                else
                    std::this_thread::yield();
            }
        };
        auto future0 = std::async(std::launch::async, runner);
        //auto future1 = std::async(std::launch::async, runner);
        taskGraph->end();
        done = true;
        future0.wait();
        //future1.wait();

        
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

void Application::createRenderingInfo()
{
    for (size_t i = 0; i < swapchain->getImageCount(); i++)
    {
        auto info = std::make_unique<sol::GraphicsRenderingInfo>();
        info->setRenderArea(0, 0, swapchain->getExtent().width, swapchain->getExtent().height);
        info->setLayerCount(1);
        info->addColorAttachment(*swapchain->getImageViews()[i],
                                 VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                 VK_ATTACHMENT_LOAD_OP_CLEAR,
                                 VK_ATTACHMENT_STORE_OP_STORE,
                                 std::array<uint32_t, 4>{0, 0, 0, 0});
        info->setColorAttachmentPreTransition(0,
                                              nullptr,
                                              nullptr,
                                              VK_IMAGE_LAYOUT_UNDEFINED,
                                              VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                              VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                              VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                              0,
                                              VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
        info->setColorAttachmentPostTransition(0,
                                               nullptr,
                                               nullptr,
                                               VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                               VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                               VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                               VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
                                               VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                               0);
        renderingInfos.emplace_back(std::move(info));
    }
}

void Application::createGeometry()
{
    const sol::GeometryBufferAllocator::Settings settings{.memoryManager = *memoryManager,
                                                          .strategy = sol::GeometryBufferAllocator::Strategy::Separate};
    geometryAllocator = sol::GeometryBufferAllocator::create(settings);

    vertexBuffer                 = geometryAllocator->allocateVertexBuffer(6, sizeof(math::float3));
    const auto       transaction = transactionManager->beginTransaction();
    const std::array vertices    = {math::float3{0.0f, 0.0f, -0.5f},
                                    math::float3{0.5f, 0.0f, 0.5f},
                                    math::float3{-0.5f, 0.0f, 0.5f},
                                    math::float3{1.0f, 0.0f, 1.0f},
                                    math::float3{-1.0f, 0.0f, 1.0f},
                                    math::float3{-1.0f, 0.0f, 0.0f}};
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

    mesh = std::make_unique<sol::Mesh>(std::move(vertexBuffer));
}

void Application::createMaterials()
{
    descriptorLayouts.emplace_back(std::make_unique<sol::DescriptorLayout>(*device));
    descriptorLayouts.back()->add(sol::DescriptorLayout::UniformBufferBinding{
      .binding = 0, .size = sizeof(math::mat4x4f), .count = 1, .stages = VK_SHADER_STAGE_VERTEX_BIT});
    descriptorLayouts.back()->add(sol::DescriptorLayout::UniformBufferBinding{
      .binding = 1, .size = sizeof(math::mat4x4f), .count = 1, .stages = VK_SHADER_STAGE_VERTEX_BIT});
    descriptorLayouts.back()->finalize();

    sol::VulkanPipelineLayout::Settings layoutSettings;
    layoutSettings.device = device;
    layoutSettings.descriptors += descriptorLayouts[0]->getLayout();
    pipelineLayout = sol::VulkanPipelineLayout::create(layoutSettings);

    sol::VulkanGraphicsPipelineVertexInput::Settings vertexInputSettings;
    vertexInputSettings.device = device;
    vertexInputSettings.vertexAttributes.emplace_back(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
    //vertexInputSettings.vertexAttributes.emplace_back(1, 0, VK_FORMAT_R32G32_SFLOAT, 12);
    vertexInputSettings.vertexBindings.emplace_back(0, 12, VK_VERTEX_INPUT_RATE_VERTEX);


    sol::VulkanGraphicsPipelinePreRasterization::Settings preRastSettings;
    preRastSettings.layout = pipelineLayout;
    preRastSettings.vertexShader.code =
      loadShaderBytecode(std::filesystem::current_path() / "gltf-viewer/shaders/viewer.vert.spv");
    preRastSettings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT);
    preRastSettings.enabledDynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT);
    preRastSettings.rasterization.cullMode = VK_CULL_MODE_NONE;


    sol::VulkanGraphicsPipelineFragment::Settings fragmentSettings;
    fragmentSettings.layout = pipelineLayout;
    fragmentSettings.fragmentShader.code =
      loadShaderBytecode(std::filesystem::current_path() / "gltf-viewer/shaders/viewer.frag.spv");
    fragmentSettings.depthStencil.depthTestEnable = false;

    sol::VulkanGraphicsPipelineFragmentOutput::Settings fragOutSettings;
    fragOutSettings.device = device;
    fragOutSettings.colorBlend.attachments.emplace_back(VK_FALSE,
                                                        VK_BLEND_FACTOR_SRC_ALPHA,
                                                        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                                                        VK_BLEND_OP_ADD,
                                                        VK_BLEND_FACTOR_ONE,
                                                        VK_BLEND_FACTOR_ZERO,
                                                        VK_BLEND_OP_ADD,
                                                        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
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
    pipelineSettings.flags            = VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
    pipelineSettings.vertexInput      = vertexInputSettings;
    pipelineSettings.preRasterization = preRastSettings;
    pipelineSettings.fragment         = fragmentSettings;
    pipelineSettings.fragmentOutput   = fragOutSettings;
    auto pipeline                     = sol::VulkanGraphicsPipeline2::create2(pipelineSettings);
#endif

    {
        const sol::DescriptorBuffer::Settings settings{
          .memoryManager = memoryManager.get(),
          .size          = 128ULL * 1024 * 1024,
          .usageflags    = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT};

        descriptorBuffer = sol::DescriptorBuffer::create(settings);

        material         = std::make_unique<sol::GraphicsMaterial2>(std::move(pipeline), raw(descriptorLayouts));
        materialInstance = material->createInstance();
    }

    {
        constexpr sol::IBufferAllocator::AllocationInfo allocation{
          .size        = sizeof(math::mat4x4f),
          .bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                         VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
          .sharingMode          = VK_SHARING_MODE_EXCLUSIVE,
          .memoryUsage          = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
          .requiredMemoryFlags  = 0,
          .preferredMemoryFlags = 0,
          .allocationFlags      = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
          .alignment            = 0};
        cameraMatrixBuffer =
          memoryManager->allocateBuffer(allocation, sol::IBufferAllocator::OnAllocationFailure::Throw);
        modelMatrixBuffer =
          memoryManager->allocateBuffer(allocation, sol::IBufferAllocator::OnAllocationFailure::Throw);

        auto& desc = materialInstance->enableDescriptor(0, *descriptorBuffer);
        desc.setUniformBuffer(*cameraMatrixBuffer, 0, 0);
        desc.setUniformBuffer(*modelMatrixBuffer, 1, 0);
    }
}

void Application::createScenegraph()
{
    scenegraph    = std::make_unique<sol::Scenegraph>();
    auto& mtlNode = scenegraph->getRootNode().addChild(std::make_unique<sol::GraphicsMaterialNode>(*materialInstance));
    auto& stateNode = mtlNode.addChild(std::make_unique<sol::GraphicsDynamicStateNode>());
    auto  viewport  = std::make_unique<sol::Viewport>();
    auto  scissor   = std::make_unique<sol::Scissor>();
    viewport->values.emplace_back(0.0f,
                                  0.0f,
                                  static_cast<float>(swapchain->getExtent().width),
                                  static_cast<float>(swapchain->getExtent().height),
                                  0.0f,
                                  1.0f);
    scissor->values.emplace_back(
      std::make_pair<int32_t, int32_t>(0, 0),
      std::pair<uint32_t, uint32_t>(swapchain->getExtent().width, swapchain->getExtent().height));
    stateNode.getStates().emplace_back(std::move(viewport));
    stateNode.getStates().emplace_back(std::move(scissor));
    stateNode.addChild(std::make_unique<sol::MeshNode>(*mesh));
}

void Application::createTaskGraph()
{
    sol::TaskGraph graph(*device, *commandPool);

    auto& acquire = graph.createTask<sol::AcquireTask>();
    acquire.setName("acquire");
    acquire.setSwapchain(*swapchain);

    /*auto& traverse = graph.createTask<sol::GraphicsTraverseTask>();
    traverse.setName("traverse");
    traverse.setScenegraph(*scenegraph);

    auto& render2 = graph.createTask<sol::GraphicsRenderTask>();
    render2.addRead(acquire.getImageIndex());
    render2.setRenderData(traverse.getRenderData());
    render2.setBufferCount(args.maxFrames);*/

    auto& render = graph.createTask<sol::RenderTask>();
    render.setName("render");
    render.addRead(acquire.getImageIndex());
    render.setBufferCount(args.maxFrames);
    render.tmpIndex    = &acquire.getImageIndex();
    render.tmpFunction = [this](sol::VulkanCommandBuffer& cb, const uint32_t imageIndex) {
        cb.resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        cb.beginOneTimeCommand();
        renderingInfos[imageIndex]->beginRendering(cb);

        sol::GraphicsTraverser  traverser;
        sol::GraphicsRenderer   renderer;
        sol::GraphicsRenderData renderData;
        traverser.setRenderData(&renderData);
        traverser.traverse(scenegraph->getRootNode());
        renderer.render({.device = *device, .renderData = renderData, .commandBuffer = cb.get()});

        renderingInfos[imageIndex]->endRendering(cb);
        cb.endCommand();
    };

    auto& submit = graph.createTask<sol::SubmitTask>();
    submit.setName("submit");
    submit.addAwait(acquire, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    submit.setQueue(memoryManager->getGraphicsQueue());
    submit.setCommandBuffer(render.getCommandBuffer());

    auto& present = graph.createTask<sol::PresentTask>();
    present.setName("present");
    present.addAwait(submit.getCommandBuffer(), 0);
    present.setQueue(memoryManager->getGraphicsQueue());
    present.setSwapchain(*swapchain, acquire.getImageIndex());
    taskGraph = graph.compile();
}
