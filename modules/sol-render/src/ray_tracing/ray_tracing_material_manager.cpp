#include "sol-render/ray_tracing/ray_tracing_material_manager.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_descriptor_pool.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_ray_tracing_pipeline.h"
#include "sol-error/sol_error.h"
#include "sol-material/ray_tracing/ray_tracing_material.h"
#include "sol-material/ray_tracing/ray_tracing_material_instance.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/common/descriptors.h"
#include "sol-render/ray_tracing/ray_tracing_pipeline_cache.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    RayTracingMaterialManager::RayTracingMaterialManager(MemoryManager& memManager) :
        memoryManager(&memManager), pipelineCache(std::make_unique<RayTracingPipelineCache>())
    {
    }

    RayTracingMaterialManager::~RayTracingMaterialManager() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    size_t RayTracingMaterialManager::getDataSetCount() const noexcept { return dataSetCount; }

    const RayTracingMaterialManager::InstanceDataMap& RayTracingMaterialManager::getInstanceData() const noexcept
    {
        return instanceDataMap;
    }

    VulkanRayTracingPipeline& RayTracingMaterialManager::getPipeline(const RayTracingMaterial& material) const
    {
        return pipelineCache->getPipeline(material);
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void RayTracingMaterialManager::setDataSetCount(const size_t count)
    {
        if (count == 0) throw SolError("Cannot set dataSetCount to 0.");
        dataSetCount = count;
    }

    ////////////////////////////////////////////////////////////////
    // Materials.
    ////////////////////////////////////////////////////////////////

    void RayTracingMaterialManager::destroyMaterial(RayTracingMaterial& material)
    {
        if (&material.getMaterialManager() != this)
            throw SolError("Cannot destroy material that is part of a different manager.");

        assert(pipelineCache->destroyPipeline(material));
        for (auto* inst : material.getInstances()) destroyMaterialInstance(*inst);
        assert(materials.erase(material.getUuid()));
    }

    void RayTracingMaterialManager::destroyMaterialInstance(RayTracingMaterialInstance& materialInstance)
    {
        if (&materialInstance.getMaterialManager() != this)
            throw SolError("Cannot destroy material instance that is part of a different manager.");

        assert(instanceDataMap.erase(&materialInstance));
        assert(materialInstances.erase(materialInstance.getUuid()));
    }

    bool RayTracingMaterialManager::createPipeline(const RayTracingMaterial& material) const
    {
        return pipelineCache->createPipeline(material);
    }

    void RayTracingMaterialManager::bindDescriptorSets(std::span<const RayTracingMaterialInstance* const> instances,
                                                       VkCommandBuffer                                    commandBuffer,
                                                       const VulkanRayTracingPipeline&                    pipeline,
                                                       size_t                                             index) const
    {
        std::vector<VkDescriptorSet> sets;
        for (const auto* mtlInstance : instances)
        {
            sets.emplace_back(instanceDataMap.find(mtlInstance)->second->descriptorSets[index]);
        }

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
                                pipeline.getPipelineLayout(),
                                0,
                                static_cast<uint32_t>(sets.size()),
                                sets.data(),
                                0,
                                nullptr);
    }

    void RayTracingMaterialManager::addMaterialImpl(RayTracingMaterialPtr material)
    {
        material->setMaterialManager(*this);
        assert(materials.try_emplace(material->getUuid(), std::move(material)).second);
    }

    void RayTracingMaterialManager::addMaterialInstanceImpl(RayTracingMaterial&           material,
                                                            RayTracingMaterialInstancePtr instance)
    {
        if (&material.getMaterialManager() != this)
            throw SolError("Cannot add material instance. Material has a different manager.");

        instance->setMaterialManager(*this);
        instance->setMaterial(material);

        const auto& mtl       = instance->getRayTracingMaterial();
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
