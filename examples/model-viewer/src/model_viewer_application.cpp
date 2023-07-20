#include "model-viewer/model_viewer_application.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <ios>
#include <iosfwd>
#include <istream>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "math/include_all.h"
#include "sol-core/object_ref_setting.h"
#include "sol-core/vulkan_bottom_level_acceleration_structure.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_command_buffer_list.h"
#include "sol-core/vulkan_command_pool.h"
#include "sol-core/vulkan_fence.h"
#include "sol-core/vulkan_physical_device_features.h"
#include "sol-core/vulkan_queue.h"
#include "sol-core/vulkan_semaphore.h"
#include "sol-core/vulkan_shader_binding_table.h"
#include "sol-core/vulkan_shader_module.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-core/vulkan_top_level_acceleration_structure.h"
#include "sol-material/graphics/i_graphics_material_manager.h"
#include "sol-material/ray_tracing/i_ray_tracing_material_manager.h"
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
#include "sol-scenegraph/node.h"
#include "sol-scenegraph/scenegraph.h"
#include "sol-scenegraph/compute/compute_material_node.h"
#include "sol-scenegraph/compute/dispatch_node.h"
#include "sol-scenegraph/drawable/mesh_node.h"
#include "sol-scenegraph/graphics/graphics_material_node.h"
#include "sol-scenegraph/ray_tracing/ray_tracing_material_node.h"
#include "sol-scenegraph/ray_tracing/trace_rays_node.h"
#include "sol-task/i_task.h"
#include "sol-task/task_graph.h"
#include "sol-task/material/graphics/update_graphics_material_manager_task.h"
#include "sol-task/material/ray_tracing/update_ray_tracing_material_manager_task.h"
#include "sol-task/other/fence_task.h"
#include "sol-task/other/submit_task.h"
#include "sol-task/present/acquire_task.h"
#include "sol-task/present/present_task.h"
#include "sol-task/render/graphics/graphics_render_task.h"
#include "sol-task/render/graphics/graphics_traverse_task.h"
#include "sol-task/render/ray_tracing/ray_tracing_render_task.h"
#include "sol-task/render/ray_tracing/ray_tracing_traverse_task.h"
#include "sol-texture/image2d.h"
#include "sol-texture/texture_manager.h"
#include "sol-texture/texture2d.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "model-viewer/materials/display_material.h"
#include "model-viewer/materials/gradient_material.h"

namespace
{
    std::vector<std::string> deviceExtensions = {};

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
                                                          sol::VulkanPhysicalDeviceVulkan13Features>>();
}

void PathtracingApplication::createEnabledFeatures()
{
    enabledFeatures = std::make_unique<sol::VulkanPhysicalDeviceFeatures2<sol::VulkanPhysicalDeviceVulkan11Features,
                                                                          sol::VulkanPhysicalDeviceVulkan12Features,
                                                                          sol::VulkanPhysicalDeviceVulkan13Features>>();

    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing  = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering    = VK_TRUE;
    enabledFeatures->getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2    = VK_TRUE;
}

std::function<bool(sol::RootVulkanPhysicalDeviceFeatures2&)> PathtracingApplication::getFeatureFilter()
{
    return [](sol::RootVulkanPhysicalDeviceFeatures2& features) {
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->bufferDeviceAddress) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan12Features>()->descriptorIndexing) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->dynamicRendering) return false;
        if (!features.getAs<sol::VulkanPhysicalDeviceVulkan13Features>()->synchronization2) return false;

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

        displayRendering.renderingInfo.resources.emplace_back(std::move(info));
    }

    displayRendering.renderingInfo.index = &imageIndex.resource;
}

void PathtracingApplication::createCommandBuffers()
{
    sol::VulkanCommandBuffer::Settings commandBufferSettings;
    commandBufferSettings.commandPool = commandPools.compute;
    commandBuffers.compute.resources  = sol::VulkanCommandBuffer::create(commandBufferSettings, args.maxFrames);
    commandBuffers.compute.index      = &frameIndex.resource;
    commandBufferSettings.commandPool = commandPools.graphics;
    commandBuffers.display.resources  = sol::VulkanCommandBuffer::create(commandBufferSettings, args.maxFrames);
    commandBuffers.display.index      = &frameIndex.resource;
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

        computeSync.inUse.index = &frameIndex.resource;
        computeSync.inUse.resources.emplace_back(sol::VulkanFence::create(fenceSettings));
        computeSync.finished.index = &frameIndex.resource;
        computeSync.finished.resources.emplace_back(sol::VulkanSemaphore::create(semSettings));

        displaySync.inUse.index = &frameIndex.resource;
        displaySync.inUse.resources.emplace_back(sol::VulkanFence::create(fenceSettings));
        displaySync.finished.index = &frameIndex.resource;
        displaySync.finished.resources.emplace_back(sol::VulkanSemaphore::create(semSettings));
        displaySync.available.index = &frameIndex.resource;
        displaySync.available.resources.emplace_back(sol::VulkanSemaphore::create(semSettings));
        displaySync.available2.resources.emplace_back(displaySync.available.resources.back().get());
        displaySync.flags.resources.emplace_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }

    displaySync.available2.test = [this](sol::VulkanSemaphore&, const size_t i) { return i == frameIndex.resource; };
    displaySync.flags.test      = [this](VkPipelineStageFlags, const size_t i) { return i == frameIndex.resource; };
}

void PathtracingApplication::createManagers()
{
    displayMaterialManager = std::make_unique<sol::GraphicsMaterialManager>(*memoryManager);
    displayMaterialManager->setDataSetCount(args.maxFrames);
    displayMaterialManager2.resource = &(*displayMaterialManager);
    meshManager                      = std::make_unique<sol::MeshManager>(*memoryManager);
    textureManager                   = std::make_unique<sol::TextureManager>(*memoryManager);
    computeMaterialManager           = std::make_unique<sol::ComputeMaterialManager>(*memoryManager);
    computeMaterialManager->setDataSetCount(args.maxFrames);
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
      std::filesystem::path("C:/Users/timzo/dev/projects/sol/source/examples/model-viewer/shaders/gradient.cs.spv"));
    shaders.displayVert = load(
      std::filesystem::path("C:/Users/timzo/dev/projects/sol/source/examples/model-viewer/shaders/display.vert.spv"));
    shaders.displayFrag = load(
      std::filesystem::path("C:/Users/timzo/dev/projects/sol/source/examples/model-viewer/shaders/display.frag.spv"));
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
                          VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                          VK_ACCESS_2_NONE,
                          VK_ACCESS_2_SHADER_SAMPLED_READ_BIT | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);
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
}

void PathtracingApplication::createScenegraphs()
{
    scenegraph     = decltype(scenegraph)::create();
    auto& rootNode = (*scenegraph).getRootNode();

    auto& computeMtlNode = rootNode.addChild(std::make_unique<sol::ComputeMaterialNode>(*computeMaterialInstance));
    computeMtlNode.addChild(std::make_unique<sol::DispatchNode>(
      texture->getImage()->getWidth() / 16, texture->getImage()->getHeight() / 16, 1));

    auto& displayMtlNode = rootNode.addChild(std::make_unique<sol::GraphicsMaterialNode>(*displayMaterialInstance));
    displayMtlNode.addChild(std::make_unique<sol::MeshNode>(*meshes.quadMesh));
}

void PathtracingApplication::createRenderers()
{
    computeRendering.renderer   = PointerTaskResource<sol::ComputeRenderer, true>::create();
    computeRendering.renderData = PointerTaskResource<sol::ComputeRenderData, true>::create();
    computeRendering.traverser  = PointerTaskResource<sol::ComputeTraverser, true>::create();
    displayRendering.renderer   = PointerTaskResource<sol::GraphicsRenderer, true>::create();
    displayRendering.renderData = PointerTaskResource<sol::GraphicsRenderData, true>::create();
    displayRendering.traverser  = PointerTaskResource<sol::GraphicsTraverser, true>::create();
}

void PathtracingApplication::createTaskGraph()
{
    taskGraph = std::make_unique<sol::TaskGraph>(*device);

    auto& displayTraverseTask = taskGraph->addTask(std::make_unique<sol::GraphicsTraverseTask>(
      "Display", &displayRendering.renderData, &displayRendering.traverser, &scenegraph));

    auto& displayAwaitFenceTask = taskGraph->addTask(std::make_unique<sol::FenceTask>(
      "Display", sol::FenceTask::Action::Wait | sol::FenceTask::Action::Reset, &displaySync.inUse));
    displayAwaitFenceTask.addDependency(displayTraverseTask);

    auto& acquireTask = taskGraph->addTask(std::make_unique<sol::AcquireTask>(
      "Acquire", &swapchain, &displaySync.available, nullptr, &imageIndex, [](sol::VulkanSwapchain&) {}));
    acquireTask.addDependency(displayAwaitFenceTask);

    auto& displayRenderTask =
      taskGraph->addTask(std::make_unique<sol::GraphicsRenderTask>("Display",
                                                                   &displayRendering.renderer,
                                                                   &displayRendering.renderData,
                                                                   &displayRendering.renderingInfo,
                                                                   &commandBuffers.display,
                                                                   &frameIndex));
    displayRenderTask.addDependency(acquireTask);

    auto& displayUpdateDataTask = taskGraph->addTask(
      std::make_unique<sol::UpdateGraphicsMaterialManagerTask>("Display", &displayMaterialManager2, &frameIndex));
    displayUpdateDataTask.addDependency(displayAwaitFenceTask);

    auto& displaySubmitTask = taskGraph->addTask(std::make_unique<sol::SubmitTask>("Display",
                                                                                   &graphicsQueue,
                                                                                   &commandBuffers.display,
                                                                                   &displaySync.available2,
                                                                                   &displaySync.flags,
                                                                                   &displaySync.finished,
                                                                                   &displaySync.inUse));
    displaySubmitTask.addDependency(displayRenderTask, displayUpdateDataTask);


    auto& presentTask = taskGraph->addTask(std::make_unique<sol::PresentTask>(
      "Present", &swapchain, &imageIndex, &graphicsQueue, &displaySync.finished, [](sol::VulkanSwapchain&) {}));
    presentTask.addDependency(displaySubmitTask);

    taskGraph->finalize();
}
