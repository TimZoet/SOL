#include "sol-render/compute/compute_material_manager.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <numeric>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_descriptor_pool.h"
#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_image_view.h"
#include "sol-core/vulkan_sampler.h"
#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-material/compute/compute_material.h"
#include "sol-material/compute/compute_material_instance.h"
#include "sol-memory/memory_manager.h"
#include "sol-texture/image2d.h"
#include "sol-texture/texture2d.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/common/descriptors.h"
#include "sol-render/compute/compute_pipeline_cache.h"

namespace
{
    void allocateDescriptorSets(const sol::VulkanDevice&                   device,
                                sol::ComputeMaterialManager::InstanceData& instanceData,
                                const size_t                               count)
    {
        const auto& materialInstance = *instanceData.materialInstance;
        const auto& material         = materialInstance.getComputeMaterial();
        const auto& mtlLayout        = material.getLayout();
        const auto  setLayout        = mtlLayout.getDescriptorSetLayouts()[materialInstance.getSetIndex()]->get();

        // Repeat layout for each set that needs to be allocated.
        const std::vector vkLayouts(count, setLayout);

        // Prepare allocation info.
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = instanceData.pool->get();
        allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
        allocInfo.pSetLayouts        = vkLayouts.data();

        // Allocate.
        instanceData.descriptorSets.resize(count);
        sol::handleVulkanError(vkAllocateDescriptorSets(device.get(), &allocInfo, instanceData.descriptorSets.data()));
    }

    void updateDescriptorSets(
      const sol::ComputeMaterialManager::InstanceData&                           instanceData,
      sol::MemoryManager&                                                        manager,
      const std::span<const sol::MaterialLayoutDescription::StorageImageBinding> storageImageBindings)
    {
        const auto perSetStorageImageInfoCount = std::accumulate(
          storageImageBindings.begin(), storageImageBindings.end(), 0, [](uint32_t sum, const auto& elem) {
              return sum + 1;
          });  // TODO: elem->count once array support is added.
        std::vector storageImageInfos(perSetStorageImageInfoCount * instanceData.descriptorSets.size(),
                                      VkDescriptorImageInfo{});

        // Create descriptor writes. Objects are laid out per set, and then per binding:
        // set<0>{ binding<0>, binding<1>, ..., binding<N> }, set<1>{...}, set<M>{ binding<0>, binding<1>, ..., binding<N>}
        const size_t bindingCount = storageImageBindings.size();
        std::vector  descriptorWrites(bindingCount * instanceData.descriptorSets.size(), VkWriteDescriptorSet{});

        for (size_t set = 0; set < instanceData.descriptorSets.size(); set++)
        {
            size_t offset = 0;

            for (size_t i = 0; i < storageImageBindings.size(); i++)
            {
                const auto& binding   = storageImageBindings[i];
                auto&       imageInfo = storageImageInfos[set * perSetStorageImageInfoCount + offset];
                auto&       texture   = *instanceData.materialInstance->getTextureData(binding.binding);
                // TODO: We use Texture2D here, which always has a sampler. While we don't access it here,
                // it seems the Texture2D class always constructs a sampler. Introduce a new, sampler-less
                // image+imageview class and use that here and in materials?
                imageInfo.imageView   = texture.getImageView()->get();
                imageInfo.imageLayout = texture.getImage()->getImageLayout();

                auto& write            = descriptorWrites[set * bindingCount + binding.binding];
                write.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.pNext            = nullptr;
                write.dstSet           = instanceData.descriptorSets[set];
                write.dstBinding       = binding.binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = 1;  // TODO: No array support yet.
                write.descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                write.pImageInfo       = storageImageInfos.data() + set * perSetStorageImageInfoCount + offset;
                write.pBufferInfo      = nullptr;
                write.pTexelBufferView = nullptr;

                offset++;
            }
        }

        vkUpdateDescriptorSets(manager.getDevice().get(),
                               static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(),
                               0,
                               nullptr);
    }
}  // namespace

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

        const auto& mtl                  = instance->getComputeMaterial();
        const auto& mtlLayout            = mtl.getLayout();
        const auto  setIndex             = instance->getSetIndex();
        const auto  storageImageBindings = mtlLayout.getStorageImages(setIndex);

        // Create new instance data.
        auto& instanceData =
          *instanceDataMap.try_emplace(instance.get(), std::make_unique<InstanceData>()).first->second;
        instanceData.materialInstance = instance.get();

        // Store material instance.
        assert(materialInstances.try_emplace(instance->getUuid(), std::move(instance)).second);

        // Create descriptor pool and allocate descriptor sets.
        instanceData.pool = createDescriptorPool(memoryManager->getDevice(), mtlLayout, dataSetCount, setIndex);
        allocateDescriptorSets(memoryManager->getDevice(), instanceData, dataSetCount);

        updateDescriptorSets(instanceData, *memoryManager, storageImageBindings);
    }


}  // namespace sol
