#include "sol-render/graphics/graphics_material_manager.h"

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
#include "sol-material/graphics/graphics_material.h"
#include "sol-material/graphics/graphics_material_instance.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/common/descriptors.h"
#include "sol-render/common/render_settings.h"
#include "sol-render/graphics/graphics_pipeline_cache.h"

namespace
{
    void createNoneUniformBuffers(
      sol::GraphicsMaterialManager::InstanceData& instanceData,
      sol::UniformBufferManager&                  manager,
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
      sol::GraphicsMaterialManager::InstanceData& instanceData,
      sol::UniformBufferManager&                  manager,
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
      sol::GraphicsMaterialManager::InstanceData& instanceData,
      sol::UniformBufferManager&                  manager,
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
      sol::GraphicsMaterialManager::InstanceData& instanceData,
      sol::UniformBufferManager&                  manager,
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

    GraphicsMaterialManager::GraphicsMaterialManager(MemoryManager& memManager) :
        memoryManager(&memManager), pipelineCache(std::make_unique<GraphicsPipelineCache>())
    {
    }

    GraphicsMaterialManager::~GraphicsMaterialManager() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    size_t GraphicsMaterialManager::getDataSetCount() const noexcept { return dataSetCount; }

    const GraphicsMaterialManager::InstanceDataMap& GraphicsMaterialManager::getInstanceData() const noexcept
    {
        return instanceDataMap;
    }

    VulkanGraphicsPipeline& GraphicsMaterialManager::getPipeline(const GraphicsMaterial& material,
                                                                 const VulkanRenderPass& renderPass) const
    {
        return pipelineCache->getPipeline(material, renderPass);
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterialManager::setDataSetCount(const size_t count)
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

    bool GraphicsMaterialManager::createPipeline(const GraphicsMaterial& material, VulkanRenderPass& renderPass) const
    {
        return pipelineCache->createPipeline(material, renderPass);
    }

    void GraphicsMaterialManager::bindDescriptorSets(std::span<const GraphicsMaterialInstance* const> instances,
                                                     VkCommandBuffer                                  commandBuffer,
                                                     const VulkanGraphicsPipeline&                    pipeline,
                                                     size_t                                           index) const
    {
        std::vector<VkDescriptorSet> sets;
        for (const auto* mtlInstance : instances)
        {
            sets.emplace_back(instanceDataMap.find(mtlInstance)->second->descriptorSets[index]);
        }

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline.getPipelineLayout(),
                                0,
                                static_cast<uint32_t>(sets.size()),
                                sets.data(),
                                0,
                                nullptr);
    }

    void GraphicsMaterialManager::destroyMaterial(GraphicsMaterial& material)
    {
        if (&material.getMaterialManager() != this)
            throw SolError("Cannot destroy material that is part of a different manager.");

        assert(pipelineCache->destroyPipeline(material));
        for (auto* inst : material.getInstances()) destroyMaterialInstance(*inst);
        assert(materials.erase(material.getUuid()));
    }

    void GraphicsMaterialManager::destroyMaterialInstance(GraphicsMaterialInstance& materialInstance)
    {
        if (&materialInstance.getMaterialManager() != this)
            throw SolError("Cannot destroy material instance that is part of a different manager.");

        assert(instanceDataMap.erase(&materialInstance));
        assert(materialInstances.erase(materialInstance.getUuid()));
    }

    ////////////////////////////////////////////////////////////////
    // Modification.
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterialManager::addMaterialImpl(GraphicsMaterialPtr material)
    {
        material->setMaterialManager(*this);
        assert(materials.try_emplace(material->getUuid(), std::move(material)).second);
    }

    void GraphicsMaterialManager::addMaterialInstanceImpl(GraphicsMaterial&           material,
                                                          GraphicsMaterialInstancePtr instance)
    {
        if (&material.getMaterialManager() != this)
            throw SolError("Cannot add material instance. Material has a different manager.");

        instance->setMaterialManager(*this);
        instance->setMaterial(material);

        const auto& mtl       = instance->getGraphicsMaterial();
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

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterialManager::updateUniformBuffers(const uint32_t index)
    {
        uniformBufferManager->mapAll(index);

        for (const auto& instanceData : instanceDataMap | std::views::values)
        {
            const auto& materialInstance = *instanceData->materialInstance;
            const auto& material         = materialInstance.getGraphicsMaterial();
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
            const auto& material         = materialInstance.getGraphicsMaterial();
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
