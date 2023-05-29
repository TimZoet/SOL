#include "sol-render/forward/forward_material_manager.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>
#include <span>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_descriptor_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_graphics_pipeline.h"
#include "sol-error/sol_error.h"
#include "sol-material/forward/forward_material.h"
#include "sol-material/forward/forward_material_instance.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/common/descriptors.h"
#include "sol-render/common/render_settings.h"

namespace
{
    void createNoneUniformBuffers(
      sol::ForwardMaterialManager::InstanceData& instanceData,
      sol::UniformBufferManager&                 manager,
      const std::vector<std::pair<const sol::MaterialLayoutDescription::UniformBufferBinding*, size_t>>&
        uniformBufferBindings)
    {
        for (const auto& [ubb, index] : uniformBufferBindings)
        {
            // Create a uniform buffer with a single slot.
            auto* uniformBuffer = manager.create(instanceData.materialInstance->getMaterial(),
                                                 instanceData.materialInstance->getSetIndex(),
                                                 ubb->binding,
                                                 sol::MaterialLayoutDescription::SharingMethod::None,
                                                 1,
                                                 ubb->size * ubb->count);

            // Acquire free slot.
            const auto [slot, offset] = uniformBuffer->getFreeSlot();

            instanceData.uniformBuffers[index] = {.uniformBuffer = uniformBuffer, .slot = slot, .offset = offset};
        }
    }

    void createInstanceUniformBuffers(
      sol::ForwardMaterialManager::InstanceData& instanceData,
      sol::UniformBufferManager&                 manager,
      const std::vector<std::pair<const sol::MaterialLayoutDescription::UniformBufferBinding*, size_t>>&
        uniformBufferBindings)
    {
        // Sum sizes of all bindings.
        size_t size = 0;
        for (const auto& ubb : uniformBufferBindings | std::views::keys) { size += ubb->size * ubb->count; }

        // Use index of first binding.
        const auto firstBinding = uniformBufferBindings.front().first->binding;

        // Create a uniform buffer with a single slot.
        auto* uniformBuffer = manager.create(instanceData.materialInstance->getMaterial(),
                                             instanceData.materialInstance->getSetIndex(),
                                             firstBinding,
                                             sol::MaterialLayoutDescription::SharingMethod::Instance,
                                             1,
                                             size);

        // Acquire free slot.
        auto [slot, offset] = uniformBuffer->getFreeSlot();

        for (const auto& [ubb, index] : uniformBufferBindings)
        {
            instanceData.uniformBuffers[index] = {.uniformBuffer = uniformBuffer, .slot = slot, .offset = offset};

            offset += ubb->size * ubb->count;
        }
    }

    void createBindingUniformBuffers(
      sol::ForwardMaterialManager::InstanceData& instanceData,
      sol::UniformBufferManager&                 manager,
      const std::vector<std::pair<const sol::MaterialLayoutDescription::UniformBufferBinding*, size_t>>&
        uniformBufferBindings)
    {
        for (const auto& [ubb, index] : uniformBufferBindings)
        {
            // Query for an existing uniform buffer with a free slot, or create a new one.
            auto* uniformBuffer = manager.getOrCreate(instanceData.materialInstance->getMaterial(),
                                                      instanceData.materialInstance->getSetIndex(),
                                                      ubb->binding,
                                                      sol::MaterialLayoutDescription::SharingMethod::Binding,
                                                      ubb->sharing.count,
                                                      ubb->size * ubb->count);

            // Acquire free slot.
            const auto [slot, offset] = uniformBuffer->getFreeSlot();

            instanceData.uniformBuffers[index] = {.uniformBuffer = uniformBuffer, .slot = slot, .offset = offset};
        }
    }

    void createInstanceAndBindingUniformBuffers(
      sol::ForwardMaterialManager::InstanceData& instanceData,
      sol::UniformBufferManager&                 manager,
      const std::vector<std::pair<const sol::MaterialLayoutDescription::UniformBufferBinding*, size_t>>&
        uniformBufferBindings)
    {
        // Sum sizes of all bindings.
        size_t size = 0;
        for (const auto& ubb : uniformBufferBindings | std::views::keys) { size += ubb->size * ubb->count; }

        // Use index and sharing count of first binding.
        const auto firstBinding = uniformBufferBindings.front().first->binding;
        const auto sharingCount = uniformBufferBindings.front().first->sharing.count;

        // Query for an existing uniform buffer with a free slot, or create a new one.
        auto* uniformBuffer = manager.getOrCreate(instanceData.materialInstance->getMaterial(),
                                                  instanceData.materialInstance->getSetIndex(),
                                                  firstBinding,
                                                  sol::MaterialLayoutDescription::SharingMethod::InstanceAndBinding,
                                                  sharingCount,
                                                  size);

        // Acquire free slot.
        auto [slot, offset] = uniformBuffer->getFreeSlot();

        for (const auto& [ubb, index] : uniformBufferBindings)
        {
            instanceData.uniformBuffers[index] = {.uniformBuffer = uniformBuffer, .slot = slot, .offset = offset};
            offset += ubb->size * ubb->count;
        }
    }
}  // namespace

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ForwardMaterialManager::ForwardMaterialManager(MemoryManager& memManager) : memoryManager(&memManager) {}

    ForwardMaterialManager::~ForwardMaterialManager() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    size_t ForwardMaterialManager::getDataSetCount() const noexcept { return dataSetCount; }

    const ForwardMaterialManager::PipelineMap& ForwardMaterialManager::getPipelines() const noexcept
    {
        return pipelines;
    }

    const ForwardMaterialManager::InstanceDataMap& ForwardMaterialManager::getInstanceData() const noexcept
    {
        return instanceDataMap;
    }

    VulkanGraphicsPipeline& ForwardMaterialManager::getPipeline(const ForwardMaterial& material,
                                                                RenderSettings&        renderSettings,
                                                                VulkanRenderPass&      renderPass) const
    {
        const auto it = pipelines.find(&material);
        if (it == pipelines.end()) throw SolError("Cannot get pipeline for ForwardMaterial: no pipelines created yet.");

        for (const auto& obj : it->second)
        {
            if (obj.renderSettings == &renderSettings && obj.renderPass == &renderPass) return *obj.pipeline;
        }

        throw SolError(
          "Cannot get pipeline for ForwardMaterial: no pipeline with compatible settings and renderpass found.");
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ForwardMaterialManager::setDataSetCount(const size_t count)
    {
        if (count == 0) throw SolError("Cannot set dataSetCount to 0.");
        dataSetCount = count;
        // TODO: Currently, this value must be set after construction, before creating any materials, and is not allowed to be changed.
        if (uniformBufferManager) throw SolError("This feature is kinda broken, see TODO above this exception :(");
        uniformBufferManager = std::make_unique<UniformBufferManager>(*memoryManager, dataSetCount);
    }

    ////////////////////////////////////////////////////////////////
    // Materials.
    ////////////////////////////////////////////////////////////////

    bool ForwardMaterialManager::createPipeline(const ForwardMaterial& material,
                                                RenderSettings&        renderSettings,
                                                VulkanRenderPass&      renderPass)
    {
        // Look for existing pipeline list for this material.
        const auto it = pipelines.find(&material);

        // None found, create whole new list with new pipeline.
        if (it == pipelines.end())
        {
            std::vector<Pipeline> p;
            p.emplace_back(Pipeline{.pipeline       = createPipelineImpl(material, renderSettings, renderPass),
                                    .renderSettings = &renderSettings,
                                    .renderPass     = &renderPass});
            pipelines.try_emplace(&material, std::move(p));
            return true;
        }

        // Look for pipeline with same settings.
        for (const auto& obj : it->second)
        {
            if (obj.renderSettings == &renderSettings && obj.renderPass == &renderPass) return false;
        }

        // Create new pipeline and add to list.
        it->second.emplace_back(Pipeline{.pipeline       = createPipelineImpl(material, renderSettings, renderPass),
                                         .renderSettings = &renderSettings,
                                         .renderPass     = &renderPass});

        return true;
    }

    void ForwardMaterialManager::destroyMaterial(ForwardMaterial& material)
    {
        if (&material.getMaterialManager() != this)
            throw SolError("Cannot destroy material that is part of a different manager.");

        assert(pipelines.erase(&material));
        for (auto* inst : material.getInstances()) destroyMaterialInstance(*inst);
        assert(materials.erase(material.getUuid()));
    }

    void ForwardMaterialManager::destroyMaterialInstance(ForwardMaterialInstance& materialInstance)
    {
        if (&materialInstance.getMaterialManager() != this)
            throw SolError("Cannot destroy material instance that is part of a different manager.");

        assert(instanceDataMap.erase(&materialInstance));
        assert(materialInstances.erase(materialInstance.getUuid()));
    }

    ////////////////////////////////////////////////////////////////
    // Modification.
    ////////////////////////////////////////////////////////////////

    void ForwardMaterialManager::addMaterialImpl(ForwardMaterialPtr material)
    {
        material->setMaterialManager(*this);
        assert(materials.try_emplace(material->getUuid(), std::move(material)).second);
    }

    void ForwardMaterialManager::addMaterialInstanceImpl(ForwardMaterial& material, ForwardMaterialInstancePtr instance)
    {
        if (&material.getMaterialManager() != this)
            throw SolError("Cannot add material instance. Material has a different manager.");

        instance->setMaterialManager(*this);
        instance->setMaterial(material);

        const auto& mtl       = instance->getForwardMaterial();
        const auto& mtlLayout = mtl.getLayout();
        const auto  setIndex  = instance->getSetIndex();

        const auto uniformBuffers = mtlLayout.getUniformBuffers(setIndex);

        // Create new instance data.
        auto& instanceData =
          *instanceDataMap.try_emplace(instance.get(), std::make_unique<InstanceData>()).first->second;
        instanceData.materialInstance = instance.get();

        // Store material instance.
        assert(materialInstances.try_emplace(instance->getUuid(), std::move(instance)).second);

        // Create descriptor pool and allocate descriptor sets.
        instanceData.pool = createDescriptorPool(memoryManager->getDevice(), mtlLayout, dataSetCount, setIndex);
        instanceData.descriptorSets =
          allocateDescriptorSets(mtlLayout.getDescriptorSetLayout(setIndex), *instanceData.pool, dataSetCount);

        // Allocate a reference to a uniform buffer for each binding.
        instanceData.uniformBuffers.resize(uniformBuffers.size());

        // Initially mark all bindings in all data sets as stale and initialize checksums to 0.
        instanceData.stale.resize(uniformBuffers.size() * dataSetCount, 1);
        instanceData.checksum.resize(uniformBuffers.size(), 0);

        // Group uniformBufferBindings by SharingMode.
        using list_t = std::vector<std::pair<const MaterialLayoutDescription::UniformBufferBinding*, size_t>>;
        list_t ubbsNone;
        list_t ubbsInstance;
        list_t ubbsBinding;
        list_t ubbsBoth;
        for (size_t i = 0; i < uniformBuffers.size(); i++)
        {
            switch (const auto& ubb = uniformBuffers[i]; ubb.sharing.method)
            {
            case MaterialLayoutDescription::SharingMethod::None: ubbsNone.emplace_back(&ubb, i); break;
            case MaterialLayoutDescription::SharingMethod::Instance: ubbsInstance.emplace_back(&ubb, i); break;
            case MaterialLayoutDescription::SharingMethod::Binding: ubbsBinding.emplace_back(&ubb, i); break;
            case MaterialLayoutDescription::SharingMethod::InstanceAndBinding: ubbsBoth.emplace_back(&ubb, i); break;
            }
        }

        // Create UniformBuffers.
        if (!ubbsNone.empty()) createNoneUniformBuffers(instanceData, *uniformBufferManager, ubbsNone);
        if (!ubbsInstance.empty()) createInstanceUniformBuffers(instanceData, *uniformBufferManager, ubbsInstance);
        if (!ubbsBinding.empty()) createBindingUniformBuffers(instanceData, *uniformBufferManager, ubbsBinding);
        if (!ubbsBoth.empty()) createInstanceAndBindingUniformBuffers(instanceData, *uniformBufferManager, ubbsBoth);

        updateDescriptorSets(instanceData.descriptorSets, *instanceData.materialInstance, instanceData.uniformBuffers);
    }

    VulkanGraphicsPipelinePtr ForwardMaterialManager::createPipelineImpl(const ForwardMaterial& material,
                                                                         RenderSettings&        renderSettings,
                                                                         VulkanRenderPass&      renderPass)
    {
        const auto* meshLayout = material.getMeshLayout();
        if (!meshLayout) throw SolError("Cannot create pipeline: material has no mesh layout.");

        // TODO: To what extent are the RenderSettings needed here? Things like culling should perhaps be put in the ForwardMaterial class instead.
        VulkanGraphicsPipeline::Settings pipelineSettings;
        pipelineSettings.renderPass   = renderPass;
        pipelineSettings.vertexShader = const_cast<VulkanShaderModule&>(material.getVertexShader());  //TODO: const_cast
        pipelineSettings.fragmentShader       = const_cast<VulkanShaderModule&>(material.getFragmentShader());
        pipelineSettings.vertexAttributes     = meshLayout->getAttributeDescriptions();
        pipelineSettings.vertexBindings       = meshLayout->getBindingDescriptions();
        pipelineSettings.descriptorSetLayouts = material.getLayout().getDescriptorSetLayouts();
        pipelineSettings.pushConstants        = material.getLayout().getPushConstants();
        pipelineSettings.colorBlending        = material.getForwardLayout().getColorBlending();

        VkPipelineRasterizationStateCreateInfo rasterization{};
        rasterization.sType     = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.lineWidth = 1.0f;
        switch (renderSettings.getPolyonMode())
        {
        case RenderSettings::PolygonMode::Fill: rasterization.polygonMode = VK_POLYGON_MODE_FILL; break;
        case RenderSettings::PolygonMode::Line: rasterization.polygonMode = VK_POLYGON_MODE_LINE; break;
        case RenderSettings::PolygonMode::Point: rasterization.polygonMode = VK_POLYGON_MODE_POINT; break;
        }
        switch (renderSettings.getCullMode())
        {
        case RenderSettings::CullMode::None: rasterization.cullMode = VK_CULL_MODE_NONE; break;
        case RenderSettings::CullMode::Front: rasterization.cullMode = VK_CULL_MODE_FRONT_BIT; break;
        case RenderSettings::CullMode::Back: rasterization.cullMode = VK_CULL_MODE_BACK_BIT; break;
        case RenderSettings::CullMode::Both: rasterization.cullMode = VK_CULL_MODE_FRONT_AND_BACK; break;
        }
        switch (renderSettings.getFrontFace())
        {
        case RenderSettings::FrontFace::Clockwise: rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE; break;
        case RenderSettings::FrontFace::CounterClockwise:
            rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;
        }

        pipelineSettings.rasterization = rasterization;

        return VulkanGraphicsPipeline::create(pipelineSettings);
    }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void ForwardMaterialManager::updateUniformBuffers(const uint32_t index)
    {
        uniformBufferManager->mapAll(index);

        for (const auto& instanceData : instanceDataMap | std::views::values)
        {
            const auto& materialInstance = *instanceData->materialInstance;
            const auto& material         = materialInstance.getForwardMaterial();
            const auto& layout           = material.getLayout();
            const auto  buffers          = layout.getUniformBuffers(materialInstance.getSetIndex());

            for (size_t i = 0; i < buffers.size(); i++)
            {
                const auto& ubb = buffers[i];

                // Skip if data is never updated.
                if (ubb.usage.updateFrequency == MaterialLayoutDescription::UpdateFrequency::Never) continue;

                bool     stale = false;
                uint32_t checksum;

                // Determine if data is stale using the required update detection method.
                switch (ubb.usage.updateDetection)
                {
                case MaterialLayoutDescription::UpdateDetection::Manual:
                    // Query material instance to see if data was updated.
                    stale = materialInstance.isUniformBufferStale(ubb.binding);
                    break;
                case MaterialLayoutDescription::UpdateDetection::Automatic:
                    // Calculate checksum of data and compare to previously recorded checksum.
                    checksum = crc32(static_cast<const uint8_t*>(materialInstance.getUniformBufferData(ubb.binding)),
                                     ubb.size * ubb.count);
                    if (checksum != instanceData->checksum[i])
                    {
                        stale                     = true;
                        instanceData->checksum[i] = checksum;
                    }
                    break;
                case MaterialLayoutDescription::UpdateDetection::Always:
                    // Always update data.
                    stale = true;
                    break;
                }

                // Mark binding as stale for all data sets.
                if (stale)
                {
                    for (size_t set = 0; set < dataSetCount; set++)
                        instanceData->stale[set * buffers.size() + i] = true;
                }
            }
        }

        for (const auto& instanceData : instanceDataMap | std::views::values)
        {
            const auto& materialInstance = *instanceData->materialInstance;
            const auto& material         = materialInstance.getForwardMaterial();
            const auto& layout           = material.getLayout();
            const auto  buffers          = layout.getUniformBuffers(materialInstance.getSetIndex());

            for (size_t i = 0; i < buffers.size(); i++)
            {
                const auto& ubb  = buffers[i];
                const auto* data = materialInstance.getUniformBufferData(ubb.binding);

                if (instanceData->stale[index * buffers.size() + i])
                {
                    const auto& tuple = instanceData->uniformBuffers[i];
                    tuple.uniformBuffer->getBuffer(index).setData(data, ubb.size * ubb.count, tuple.offset);
                    instanceData->stale[index * buffers.size() + i] = false;
                }
            }
        }

        uniformBufferManager->unmapAll(index);
        uniformBufferManager->flushAll(index);
    }
}  // namespace sol
