#include "sol-render/compute/compute_material_manager.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_compute_pipeline.h"
#include "sol-core/vulkan_descriptor_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-error/sol_error.h"
#include "sol-material/compute/compute_material.h"
#include "sol-material/compute/compute_material_instance.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/common/descriptors.h"
#include "sol-render/compute/compute_pipeline_cache.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ComputeMaterialManager::ComputeMaterialManager(MemoryManager& memManager) :
        memoryManager(&memManager), pipelineCache(std::make_unique<ComputePipelineCache>())
    {
    }

    ComputeMaterialManager::~ComputeMaterialManager() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    size_t ComputeMaterialManager::getDataSetCount() const noexcept { return dataSetCount; }

    const ComputeMaterialManager::InstanceDataMap& ComputeMaterialManager::getInstanceData() const noexcept
    {
        return instanceDataMap;
    }

    VulkanComputePipeline& ComputeMaterialManager::getPipeline(const ComputeMaterial& material) const
    {
        return pipelineCache->getPipeline(material);
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ComputeMaterialManager::setDataSetCount(const size_t count)
    {
        if (count == 0) throw SolError("Cannot set dataSetCount to 0.");
        dataSetCount = count;
    }

    ////////////////////////////////////////////////////////////////
    // Materials.
    ////////////////////////////////////////////////////////////////

    void ComputeMaterialManager::destroyMaterial(ComputeMaterial& material)
    {
        if (&material.getMaterialManager() != this)
            throw SolError("Cannot destroy material that is part of a different manager.");

        assert(pipelineCache->destroyPipeline(material));
        for (auto* inst : material.getInstances()) destroyMaterialInstance(*inst);
        assert(materials.erase(material.getUuid()));
    }

    void ComputeMaterialManager::destroyMaterialInstance(ComputeMaterialInstance& materialInstance)
    {
        if (&materialInstance.getMaterialManager() != this)
            throw SolError("Cannot destroy material instance that is part of a different manager.");

        assert(instanceDataMap.erase(&materialInstance));
        assert(materialInstances.erase(materialInstance.getUuid()));
    }

    bool ComputeMaterialManager::createPipeline(const ComputeMaterial& material) const
    {
        return pipelineCache->createPipeline(material);
    }

    void ComputeMaterialManager::bindDescriptorSets(std::span<const ComputeMaterialInstance* const> instances,
                                                    VkCommandBuffer                                 commandBuffer,
                                                    const VulkanComputePipeline&                    pipeline,
                                                    const size_t                                    index) const
    {
        std::vector<VkDescriptorSet> sets;
        for (const auto* mtlInstance : instances)
        {
            sets.emplace_back(instanceDataMap.find(mtlInstance)->second->descriptorSets[index]);
        }

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_COMPUTE,
                                pipeline.getPipelineLayout(),
                                0,
                                static_cast<uint32_t>(sets.size()),
                                sets.data(),
                                0,
                                nullptr);
    }

    void ComputeMaterialManager::addMaterialImpl(ComputeMaterialPtr material)
    {
        material->setMaterialManager(*this);
        assert(materials.try_emplace(material->getUuid(), std::move(material)).second);
    }

    void ComputeMaterialManager::addMaterialInstanceImpl(ComputeMaterial& material, ComputeMaterialInstancePtr instance)
    {
        if (&material.getMaterialManager() != this)
            throw SolError("Cannot add material instance. Material has a different manager.");

        instance->setMaterialManager(*this);
        instance->setMaterial(material);

        const auto& mtl       = instance->getComputeMaterial();
        const auto& mtlLayout = mtl.getLayout();
        const auto  setIndex  = instance->getSetIndex();

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

        const std::vector<UniformBufferManager::UniformBufferReference> ubs;
        updateDescriptorSets(instanceData.descriptorSets, *instanceData.materialInstance, ubs);
    }
}  // namespace sol
