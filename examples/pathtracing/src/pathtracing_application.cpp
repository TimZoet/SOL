#include "pathtracing/pathtracing_application.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <fstream>
#include <future>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "math/include_all.h"
#include "sol-core/vulkan_bottom_level_acceleration_structure.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_compute_pipeline.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_shader_binding_table.h"
#include "sol-core/vulkan_shader_module.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-core/vulkan_top_level_acceleration_structure.h"
#include "sol-material/compute/compute_material.h"
#include "sol-material/compute/compute_material_instance.h"
#include "sol-material/graphics/graphics_material.h"
#include "sol-material/ray_tracing/ray_tracing_material.h"
#include "sol-material/ray_tracing/ray_tracing_material_instance.h"
#include "sol-memory/memory_manager.h"
#include "sol-mesh/indexed_mesh.h"
#include "sol-mesh/mesh_description.h"
#include "sol-mesh/mesh_layout.h"
#include "sol-mesh/mesh_manager.h"
#include "sol-render/compute/compute_material_manager.h"
#include "sol-render/compute/compute_renderer.h"
#include "sol-render/compute/compute_render_data.h"
#include "sol-render/compute/compute_traverser.h"
#include "sol-render/graphics/graphics_material_manager.h"
#include "sol-render/graphics/graphics_renderer.h"
#include "sol-render/graphics/graphics_rendering_info.h"
#include "sol-render/graphics/graphics_render_data.h"
#include "sol-render/graphics/graphics_traverser.h"
#include "sol-render/ray_tracing/ray_tracing_material_manager.h"
#include "sol-render/ray_tracing/ray_tracing_renderer.h"
#include "sol-render/ray_tracing/ray_tracing_render_data.h"
#include "sol-render/ray_tracing/ray_tracing_traverser.h"
#include "sol-scenegraph/scenegraph.h"
#include "sol-scenegraph/compute/compute_material_node.h"
#include "sol-scenegraph/compute/dispatch_node.h"
#include "sol-scenegraph/drawable/mesh_node.h"
#include "sol-scenegraph/graphics/graphics_material_node.h"
#include "sol-scenegraph/ray_tracing/ray_tracing_material_node.h"
#include "sol-scenegraph/ray_tracing/trace_rays_node.h"
#include "sol-task/task_graph.h"
#include "sol-task/material/graphics/update_graphics_material_manager_task.h"
#include "sol-task/material/ray_tracing/update_ray_tracing_material_manager_task.h"
#include "sol-task/other/custom_task.h"
#include "sol-task/other/fence_task.h"
#include "sol-task/other/submit_task.h"
#include "sol-task/present/acquire_task.h"
#include "sol-task/present/present_task.h"
#include "sol-task/render/graphics/graphics_render_task.h"
#include "sol-task/render/graphics/graphics_traverse_task.h"
#include "sol-task/render/ray_tracing/ray_tracing_render_task.h"
#include "sol-task/render/ray_tracing/ray_tracing_traverse_task.h"
#include "sol-texture/image2d.h"
#include "sol-texture/texture2d.h"
#include "sol-texture/texture_manager.h"
#include "sol-window/window.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/materials/display_material.h"
#include "pathtracing/materials/gradient_material.h"
#include "pathtracing/materials/rt_material.h"
#include "pathtracing/tasks/function_task_resource_list.h"
#include "pathtracing/tasks/function_task_resource_list_2.h"
#include "pathtracing/tasks/indexed_command_buffer_list_resource_list.h"
#include "pathtracing/tasks/indexed_task_resource.h"
#include "pathtracing/tasks/indexed_task_resource_list.h"
#include "pathtracing/tasks/pointer_task_resource.h"

namespace
{
    std::vector<std::string> deviceExtensions = {VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                                                 VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,

                                                 VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
                                                 VK_KHR_SPIRV_1_4_EXTENSION_NAME,
                                                 VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
                                                 VK_KHR_RAY_QUERY_EXTENSION_NAME};

    struct Vertex
    {
        math::float3 pos;
    };

}  // namespace

PathtracingApplication::PathtracingApplication() = default;

PathtracingApplication::~PathtracingApplication() noexcept = default;

void PathtracingApplication::initialize()
{
    BaseApplication::initialize();
    createRenderingInfos();
    createCommandBuffers();
    createSynchronization();
    createManagers();
    createShaders();
    createMeshes();
    createAccelerationStructures();
    createTextures();
    createMaterials();
    createScenegraphs();
    createRenderers();
    createTaskGraph();
}

std::vector<std::string> PathtracingApplication::getExtensions() { return deviceExtensions; }

void PathtracingApplication::createSupportedFeatures()
{
    supportedFeatures =
      std::make_unique<sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                                          sol::VulkanPhysicalDeviceVulkan12Features,
                                                          sol::VulkanPhysicalDeviceVulkan13Features,
                                                          sol::VulkanPhysicalDeviceAccelerationStructureFeaturesKHR,
                                                          sol::VulkanPhysicalDeviceRayQueryFeaturesKHR,
                                                          sol::VulkanPhysicalDeviceRayTracingPipelineFeaturesKHR>>();
}

void PathtracingApplication::createEnabledFeatures()
{
    enabledFeatures =
      std::make_unique<sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                                          sol::VulkanPhysicalDeviceVulkan12Features,
                                                          sol::VulkanPhysicalDeviceVulkan13Features,
                                                          sol::VulkanPhysicalDeviceAccelerationStructureFeaturesKHR,
                                                          sol::VulkanPhysicalDeviceRayQueryFeaturesKHR,
                                                          sol::VulkanPhysicalDeviceRayTracingPipelineFeaturesKHR>>();

    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing  = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering    = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2    = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceAccelerationStructureFeaturesKHR>()->accelerationStructure =
      VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceRayQueryFeaturesKHR>()->rayQuery                     = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceRayTracingPipelineFeaturesKHR>()->rayTracingPipeline = VK_TRUE;
}

std::function<bool(sol::RootVulkanPhysicalDeviceFeatures2&)> PathtracingApplication::getFeatureFilter()
{
    return [](sol::RootVulkanPhysicalDeviceFeatures2& features) {
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceAccelerationStructureFeaturesKHR>()->accelerationStructure)
            return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceRayQueryFeaturesKHR>()->rayQuery) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceRayTracingPipelineFeaturesKHR>()->rayTracingPipeline) return false;

        return true;
    };
}

void PathtracingApplication::createRenderingInfos()
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
        info->finalize();
        displayRendering.renderingInfo.emplace_back(std::move(info));
    }
}

void PathtracingApplication::createCommandBuffers()
{
    sol::VulkanCommandBuffer::Settings commandBufferSettings;
    commandBufferSettings.commandPool = commandPools.compute;
    commandBuffers.compute            = sol::VulkanCommandBuffer::create(commandBufferSettings, args.maxFrames);
    commandBufferSettings.commandPool = commandPools.graphics;
    commandBuffers.display            = sol::VulkanCommandBuffer::create(commandBufferSettings, args.maxFrames);
    commandBuffers.rt                 = sol::VulkanCommandBuffer::create(commandBufferSettings, args.maxFrames);
}

void PathtracingApplication::createSynchronization()
{
    for (uint32_t i = 0; i < args.maxFrames; i++)
    {
        sol::VulkanFence::Settings fenceSettings;
        fenceSettings.device   = device;
        fenceSettings.signaled = true;
        sol::VulkanSemaphore::Settings semSettings;
        semSettings.device = device;

        computeSync.inUse.emplace_back(sol::VulkanFence::create(fenceSettings));
        computeSync.finished.emplace_back(sol::VulkanSemaphore::create(semSettings));

        displaySync.inUse.emplace_back(sol::VulkanFence::create(fenceSettings));
        displaySync.finished.emplace_back(sol::VulkanSemaphore::create(semSettings));
        displaySync.available.emplace_back(sol::VulkanSemaphore::create(semSettings));

        rtSync.inUse.emplace_back(sol::VulkanFence::create(fenceSettings));
        rtSync.finished.emplace_back(sol::VulkanSemaphore::create(semSettings));
    }
}

void PathtracingApplication::createManagers()
{
    displayMaterialManager = std::make_unique<sol::GraphicsMaterialManager>(*memoryManager);
    displayMaterialManager->setDataSetCount(args.maxFrames);
    meshManager            = std::make_unique<sol::MeshManager>(*memoryManager);
    textureManager         = std::make_unique<sol::TextureManager>(*memoryManager);
    computeMaterialManager = std::make_unique<sol::ComputeMaterialManager>(*memoryManager);
    computeMaterialManager->setDataSetCount(args.maxFrames);
    rtMaterialManager = std::make_unique<sol::RayTracingMaterialManager>(*memoryManager);
    rtMaterialManager->setDataSetCount(args.maxFrames);
}

void PathtracingApplication::createShaders()
{
    const auto load = [this](const std::filesystem::path& filename) {
        sol::VulkanShaderModule::Settings settings;
        settings.device = device;
        settings.type   = sol::VulkanShaderModule::ShaderType::Compute;
        std::ifstream file(filename, std::ios::binary);
        file.seekg(0, std::ios::end);
        const auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        settings.binary.resize(size);
        file.read(reinterpret_cast<char*>(settings.binary.data()), size);

        return sol::VulkanShaderModule::create(settings);
    };

    // TODO: Retrieve from build directory.
    shaders.gradientCs = load(
      std::filesystem::path("C:/Users/timzo/dev/projects/sol/source/examples/pathtracing/shaders/gradient.cs.spv"));
    shaders.displayVert = load(
      std::filesystem::path("C:/Users/timzo/dev/projects/sol/source/examples/pathtracing/shaders/display.vert.spv"));
    shaders.displayFrag = load(
      std::filesystem::path("C:/Users/timzo/dev/projects/sol/source/examples/pathtracing/shaders/display.frag.spv"));
    shaders.raygen = load(
      std::filesystem::path("C:/Users/timzo/dev/projects/sol/source/examples/pathtracing/shaders/raygen.rgen.spv"));
    shaders.miss =
      load(std::filesystem::path("C:/Users/timzo/dev/projects/sol/source/examples/pathtracing/shaders/miss.rmiss.spv"));
    shaders.closesthit = load(std::filesystem::path(
      "C:/Users/timzo/dev/projects/sol/source/examples/pathtracing/shaders/closesthit.rchit.spv"));
}

void PathtracingApplication::createMeshes()
{
    meshes.displayMeshLayout = &meshManager->createMeshLayout("display");
    meshes.displayMeshLayout->addBinding("vertex", 0, 16, VK_VERTEX_INPUT_RATE_VERTEX);
    meshes.displayMeshLayout->addAttribute("pos", 0, 0, VK_FORMAT_R32G32_SFLOAT, 0);
    meshes.displayMeshLayout->addAttribute("uv", 1, 0, VK_FORMAT_R32G32_SFLOAT, 8);
    meshes.displayMeshLayout->finalize();

    {
        auto quadDescription = meshManager->createMeshDescription();
        quadDescription->addVertexBuffer(sizeof(math::float2) * 2, 4);
        quadDescription->addIndexBuffer(sizeof(int16_t), 6);

        const std::array quadVerts = {math::float2(-1, -1),
                                      math::float2(0, 0),
                                      math::float2(1, -1),
                                      math::float2(1, 0),
                                      math::float2(1, 1),
                                      math::float2(1, 1),
                                      math::float2(-1, 1),
                                      math::float2(0, 1)};

        const std::array<int16_t, 6> quadIndices = {0, 1, 2, 2, 3, 0};

        quadDescription->setVertexData(0, 0, 4, quadVerts.data());
        quadDescription->setIndexData(0, 6, quadIndices.data());

        meshes.quadMesh = &meshManager->createIndexedMesh(std::move(quadDescription));
    }

    meshes.triangleMeshLayout = &meshManager->createMeshLayout("triangle");
    meshes.triangleMeshLayout->addBinding("vertex", 0, 16, VK_VERTEX_INPUT_RATE_VERTEX);
    meshes.triangleMeshLayout->addAttribute("pos", 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
    meshes.triangleMeshLayout->finalize();

    {
        auto desc = meshManager->createMeshDescription();
        desc->addVertexBuffer(sizeof(Vertex),
                              3,
                              0,
                              VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
        desc->addIndexBuffer(sizeof(int16_t),
                             3,
                             0,
                             VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                               VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);

        const std::array verts = {math::float3(1, 1, 0), math::float3(-1, 1, 0), math::float3(0, -1, 0)};

        const std::array<int16_t, 3> indices = {0, 1, 2};

        desc->setVertexData(0, 0, 3, verts.data());
        desc->setIndexData(0, 3, indices.data());

        meshes.triangleMesh = &meshManager->createIndexedMesh(std::move(desc));
    }

    meshManager->transferStagedCopies();
}

void PathtracingApplication::createAccelerationStructures()
{
    {
        auto& queue = memoryManager->getComputeQueue();
        auto cb = memoryManager->getCommandPool(queue.getFamily()).createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        sol::VulkanBottomLevelAccelerationStructure::Settings settings;
        settings.queue                = queue;
        settings.commandBuffer        = cb;
        settings.allocator            = memoryManager->getAllocator();
        settings.vertexBuffer         = meshes.triangleMesh->getVertexBuffer();
        settings.indexBuffer          = meshes.triangleMesh->getIndexBuffer();
        settings.numTriangles         = meshes.triangleMesh->getIndexCount() / 3;
        settings.maxVertex            = meshes.triangleMesh->getVertexCount();
        settings.vertexStride         = sizeof(Vertex);
        accelerationStructures.bottom = sol::VulkanBottomLevelAccelerationStructure::create(settings);
        cb->resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }

    {
        auto& queue = memoryManager->getComputeQueue();
        auto cb = memoryManager->getCommandPool(queue.getFamily()).createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        sol::VulkanTopLevelAccelerationStructure::Settings settings;
        settings.queue             = queue;
        settings.commandBuffer     = cb;
        settings.allocator         = memoryManager->getAllocator();
        settings.bottomLevelAS     = accelerationStructures.bottom;
        accelerationStructures.top = sol::VulkanTopLevelAccelerationStructure::create(settings);
        cb->resetCommand(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }
}

void PathtracingApplication::createTextures()
{
    auto& image = textureManager->createImage2D(VK_FORMAT_R32G32B32A32_SFLOAT,
                                                {args.width, args.height},
                                                VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    image.createStagingBuffer({0, 0}, {args.width, args.height});
    constexpr auto data = std::array<float, 4>{1, 1, 1, 1};
    image.fill(data.data(), 0);
    image.setQueueFamily(memoryManager->getGraphicsQueue().getFamily());
    //image.setData()
    image.stageTransition(&memoryManager->getGraphicsQueue().getFamily(),
                          VK_IMAGE_LAYOUT_GENERAL,
                          VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
                          VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR,
                          VK_ACCESS_2_NONE,
                          VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT);
    texture = &textureManager->createTexture2D(image);
    textureManager->transfer();

    /*image.stageTransition(&memoryManager->getGraphicsQueue().getFamily(),
                          VK_IMAGE_LAYOUT_GENERAL,
                          VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR,
                          VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                          VK_ACCESS_2_NONE,
                          VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);
    texture = &textureManager->createTexture2D(image);
    textureManager->transfer();*/
}

void PathtracingApplication::createMaterials()
{
    computeMaterial = &computeMaterialManager->addMaterial(std::make_unique<GradientMaterial>(*shaders.gradientCs));
    computeMaterialInstance = &computeMaterialManager->addMaterialInstance(
      *computeMaterial, std::make_unique<GradientMaterialInstance>(*texture));

    displayMaterial = &displayMaterialManager->addMaterial(
      std::make_unique<DisplayMaterial>(*meshes.displayMeshLayout, *shaders.displayVert, *shaders.displayFrag));
    displayMaterialInstance = &displayMaterialManager->addMaterialInstance(
      *displayMaterial, std::make_unique<DisplayMaterialInstance>(math::uint2{args.width, args.height}, *texture));

    rtMaterial = &rtMaterialManager->addMaterial(
      std::make_unique<RTMaterial>(*shaders.raygen, *shaders.miss, *shaders.closesthit));
    rtMaterialInstance = &rtMaterialManager->addMaterialInstance(
      *rtMaterial, std::make_unique<RTMaterialInstance>(*accelerationStructures.top, *texture));
    static_cast<void>(rtMaterialManager->createPipeline(*rtMaterial));
}

void PathtracingApplication::createScenegraphs()
{
    scenegraph     = std::make_unique<sol::Scenegraph>();
    auto& rootNode = scenegraph->getRootNode();

    auto& computeMtlNode = rootNode.addChild(std::make_unique<sol::ComputeMaterialNode>(*computeMaterialInstance));
    computeMtlNode.addChild(std::make_unique<sol::DispatchNode>(
      texture->getImage()->getWidth() / 16, texture->getImage()->getHeight() / 16, 1));

    auto& displayMtlNode = rootNode.addChild(std::make_unique<sol::GraphicsMaterialNode>(*displayMaterialInstance));
    //displayMtlNode.addChild(std::make_unique<sol::MeshNode>(*meshes.quadMesh));
    //displayMtlNode.addChild(std::make_unique<sol::MeshNode>(*meshes.quadMesh));
    //displayMtlNode.addChild(std::make_unique<sol::MeshNode>(*meshes.quadMesh));

    auto&                                   pipeline = rtMaterialManager->getPipeline(*rtMaterial);
    sol::VulkanShaderBindingTable::Settings settings;
    settings.allocator = memoryManager->getAllocator();
    settings.device    = device;
    settings.pipeline  = pipeline;
    sbt                = sol::VulkanShaderBindingTable::create(settings);
    auto& rtMtlNode    = rootNode.addChild(std::make_unique<sol::RayTracingMaterialNode>(*rtMaterialInstance));
    rtMtlNode.addChild(std::make_unique<sol::TraceRaysNode>(
      sbt->getRaygenRegion(),
      sbt->getMissRegion(),
      sbt->getHitRegion(),
      sbt->getCallableRegion(),
      std::array<uint32_t, 3>{texture->getImage()->getWidth(), texture->getImage()->getHeight(), 1}));
}

void PathtracingApplication::createRenderers()
{
    computeRendering.renderer   = std::make_unique<sol::ComputeRenderer>();
    computeRendering.renderData = std::make_unique<sol::ComputeRenderData>();
    computeRendering.traverser  = std::make_unique<sol::ComputeTraverser>();
    displayRendering.renderer   = std::make_unique<sol::GraphicsRenderer>();
    displayRendering.renderData = std::make_unique<sol::GraphicsRenderData>();
    displayRendering.traverser  = std::make_unique<sol::GraphicsTraverser>();
    rtRendering.renderer        = std::make_unique<sol::RayTracingRenderer>();
    rtRendering.renderData      = std::make_unique<sol::RayTracingRenderData>();
    rtRendering.traverser       = std::make_unique<sol::RayTracingTraverser>();
}

void PathtracingApplication::createTaskGraph()
{
    taskGraph = std::make_unique<sol::TaskGraph>();

#if 1
    auto& rtTraverseTask = taskGraph->createTask<sol::RayTracingTraverseTask>(
      std::make_unique<PointerTaskResource<sol::RayTracingRenderData>>(*rtRendering.renderData),
      std::make_unique<PointerTaskResource<sol::RayTracingTraverser>>(*rtRendering.traverser),
      std::make_unique<PointerTaskResource<sol::Scenegraph>>(*scenegraph));
    rtTraverseTask.setName("(RT)");

    auto& rtAwaitFenceTask = taskGraph->createTask<sol::FenceTask>(
      sol::FenceTask::Action::Wait | sol::FenceTask::Action::Reset,
      std::make_unique<IndexedTaskResourceList<sol::VulkanFence, uint32_t>>(rtSync.inUse, frameIndex));
    rtAwaitFenceTask.addDependency(rtTraverseTask);

    auto& rtRenderTask = taskGraph->createTask<sol::RayTracingRenderTask>(
      std::make_unique<PointerTaskResource<sol::RayTracingRenderer>>(*rtRendering.renderer),
      std::make_unique<PointerTaskResource<sol::RayTracingRenderData>>(*rtRendering.renderData),
      std::make_unique<PointerTaskResource<sol::VulkanCommandBufferList>>(*commandBuffers.rt),
      std::make_unique<PointerTaskResource<uint32_t>>(frameIndex));
    rtRenderTask.setName("(RT)");
    rtRenderTask.addDependency(rtAwaitFenceTask);

    auto& rtUpdateDataTask = taskGraph->createTask<sol::UpdateRayTracingMaterialManagerTask>(
      std::make_unique<PointerTaskResource<sol::IRayTracingMaterialManager>>(*rtMaterialManager),
      std::make_unique<PointerTaskResource<uint32_t>>(frameIndex));
    rtUpdateDataTask.addDependency(rtAwaitFenceTask);

    auto& rtSubmitTask = taskGraph->createTask<sol::SubmitTask>();
    rtSubmitTask.setName("(RT)");
    rtSubmitTask.submitQueue =
      std::make_unique<PointerTaskResource<sol::VulkanQueue>>(memoryManager->getGraphicsQueue());
    rtSubmitTask.commandBuffers =
      std::make_unique<IndexedCommandBufferListResourceList<uint32_t>>(*commandBuffers.rt, frameIndex);
    rtSubmitTask.signalSemaphores =
      std::make_unique<IndexedTaskResourceList<sol::VulkanSemaphore, uint32_t>>(rtSync.finished, frameIndex);
    rtSubmitTask.signalFence =
      std::make_unique<IndexedTaskResource<sol::VulkanFence, uint32_t>>(rtSync.inUse, frameIndex);
    rtSubmitTask.addDependency(rtRenderTask, rtUpdateDataTask);
#endif

    /*
     * Display / Presentation.
     */

    auto& displayTraverseTask = taskGraph->createTask<sol::GraphicsTraverseTask>(
      std::make_unique<PointerTaskResource<sol::GraphicsRenderData>>(*displayRendering.renderData),
      std::make_unique<PointerTaskResource<sol::GraphicsTraverser>>(*displayRendering.traverser),
      std::make_unique<PointerTaskResource<sol::Scenegraph>>(*scenegraph));
    displayTraverseTask.setName("(Display)");

    auto& displayAwaitFenceTask = taskGraph->createTask<sol::FenceTask>(
      sol::FenceTask::Action::Wait | sol::FenceTask::Action::Reset,
      std::make_unique<IndexedTaskResourceList<sol::VulkanFence, uint32_t>>(displaySync.inUse, frameIndex));
    displayAwaitFenceTask.setName("(Display) Await Fence");
    displayAwaitFenceTask.addDependency(displayTraverseTask);

    auto& acquireTask = taskGraph->createTask<sol::AcquireTask>(
      std::make_unique<PointerTaskResource<sol::VulkanSwapchain>>(*swapchain),
      std::make_unique<IndexedTaskResource<sol::VulkanSemaphore, uint32_t>>(displaySync.available, frameIndex),
      nullptr,
      std::make_unique<PointerTaskResource<uint32_t>>(imageIndex),
      [](sol::VulkanSwapchain&) {});
    acquireTask.setName("(Display)");
    acquireTask.addDependency(displayAwaitFenceTask);

    auto& displayRenderTask = taskGraph->createTask<sol::GraphicsRenderTask>(
      std::make_unique<PointerTaskResource<sol::GraphicsRenderer>>(*displayRendering.renderer),
      std::make_unique<PointerTaskResource<sol::GraphicsRenderData>>(*displayRendering.renderData),
      std::make_unique<IndexedTaskResource<sol::GraphicsRenderingInfo, uint32_t>>(displayRendering.renderingInfo,
                                                                                  imageIndex),
      std::make_unique<IndexedTaskResource<sol::VulkanCommandBuffer>>(commandBuffers.display, frameIndex),
      std::make_unique<PointerTaskResource<uint32_t>>(frameIndex));
    displayRenderTask.setName("(Display)");
    displayRenderTask.addDependency(acquireTask);

    auto& displayUpdateDataTask = taskGraph->createTask<sol::UpdateGraphicsMaterialManagerTask>(
      std::make_unique<PointerTaskResource<sol::IGraphicsMaterialManager>>(*displayMaterialManager),
      std::make_unique<PointerTaskResource<uint32_t>>(frameIndex));
    displayUpdateDataTask.setName("(Display)");
    displayUpdateDataTask.addDependency(displayAwaitFenceTask);

    auto& displaySubmitTask = taskGraph->createTask<sol::SubmitTask>();
    displaySubmitTask.setName("(Display)");
    displaySubmitTask.submitQueue =
      std::make_unique<PointerTaskResource<sol::VulkanQueue>>(memoryManager->getGraphicsQueue());
    displaySubmitTask.commandBuffers =
      std::make_unique<IndexedCommandBufferListResourceList<uint32_t>>(*commandBuffers.display, frameIndex);

    std::vector<sol::VulkanSemaphore*> sems;
    std::vector<VkPipelineStageFlags>  flags;
    for (size_t i = 0; i < args.maxFrames; i++)
    {
        sems.push_back(rtSync.finished[i].get());
        sems.push_back(displaySync.available[i].get());
        flags.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        flags.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }

    displaySubmitTask.waitSemaphores = std::make_unique<FunctionTaskResourceList<sol::VulkanSemaphore>>(
      sems, [this](const size_t i) { return i / 2 == frameIndex; });
    displaySubmitTask.waitFlags = std::make_unique<FunctionTaskResourceList2<VkPipelineStageFlags>>(
      flags, [this](const size_t i) { return i / 2 == frameIndex; });
    displaySubmitTask.signalSemaphores =
      std::make_unique<IndexedTaskResourceList<sol::VulkanSemaphore, uint32_t>>(displaySync.finished, frameIndex);
    displaySubmitTask.signalFence =
      std::make_unique<IndexedTaskResource<sol::VulkanFence, uint32_t>>(displaySync.inUse, frameIndex);
    displaySubmitTask.addDependency(displayRenderTask, displayUpdateDataTask, rtSubmitTask);


    auto& presentTask = taskGraph->createTask<sol::PresentTask>(
      std::make_unique<PointerTaskResource<sol::VulkanSwapchain>>(*swapchain),
      std::make_unique<PointerTaskResource<uint32_t>>(imageIndex),
      std::make_unique<PointerTaskResource<sol::VulkanQueue>>(memoryManager->getGraphicsQueue()),
      std::make_unique<IndexedTaskResourceList<sol::VulkanSemaphore, uint32_t>>(displaySync.finished, frameIndex),
      [](sol::VulkanSwapchain&) {});
    presentTask.setName("(Present)");
    presentTask.addDependency(displaySubmitTask);

    taskGraph->finalize();
}
