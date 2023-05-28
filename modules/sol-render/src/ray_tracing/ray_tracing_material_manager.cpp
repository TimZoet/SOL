#include "sol-render/ray_tracing/ray_tracing_material_manager.h"

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
#include "sol-core/vulkan_graphics_pipeline.h"
#include "sol-core/vulkan_top_level_acceleration_structure.h"
#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-material/ray_tracing/ray_tracing_material.h"
#include "sol-material/ray_tracing/ray_tracing_material_instance.h"
#include "sol-memory/memory_manager.h"
#include "sol-texture/image2d.h"
#include "sol-texture/texture2d.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/ray_tracing/ray_tracing_pipeline_cache.h"

namespace
{
    [[nodiscard]] sol::VulkanDescriptorPoolPtr
      createDescriptorPool(sol::VulkanDevice& device, const sol::RayTracingMaterialLayout& layout, const size_t count)
    {
        sol::VulkanDescriptorPool::Settings poolSettings;
        poolSettings.device  = device;
        poolSettings.maxSets = static_cast<uint32_t>(count);

        // Storage images.
        if (layout.getStorageImageCount() > 0)
        {
            poolSettings.poolSizes.emplace_back(
              VkDescriptorPoolSize{.type            = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                   .descriptorCount = static_cast<uint32_t>(count * layout.getStorageImageCount())});
        }

        // Storage buffers.
        if (layout.getStorageBufferCount() > 0)
        {
            poolSettings.poolSizes.emplace_back(
              VkDescriptorPoolSize{.type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                   .descriptorCount = static_cast<uint32_t>(count * layout.getStorageBufferCount())});
        }

        // Acceleration structures.
        if (layout.getAccelerationStructureCount() > 0)
        {
            poolSettings.poolSizes.emplace_back(VkDescriptorPoolSize{
              .type            = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
              .descriptorCount = static_cast<uint32_t>(count * layout.getAccelerationStructureCount())});
        }

        return sol::VulkanDescriptorPool::create(poolSettings);
    }

    void allocateDescriptorSets(const sol::VulkanDevice&                      device,
                                sol::RayTracingMaterialManager::InstanceData& instanceData,
                                const size_t                                  count)
    {
        const auto& materialInstance = *instanceData.materialInstance;
        const auto& material         = materialInstance.getRayTracingMaterial();
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
      const sol::RayTracingMaterialManager::InstanceData&                                 instanceData,
      sol::MemoryManager&                                                                 manager,
      const std::span<const sol::MaterialLayoutDescription::StorageImageBinding>          storageImageBindings,
      const std::span<const sol::MaterialLayoutDescription::StorageBufferBinding>         storageBufferBindings,
      const std::span<const sol::MaterialLayoutDescription::AccelerationStructureBinding> accelerationStructureBindings)
    {
        const auto perSetStorageImageInfoCount = std::accumulate(
          storageImageBindings.begin(), storageImageBindings.end(), 0, [](uint32_t sum, const auto& elem) {
              return sum + 1;
          });  // TODO: elem->count once array support is added.
        std::vector storageImageInfos(perSetStorageImageInfoCount * instanceData.descriptorSets.size(),
                                      VkDescriptorImageInfo{});

        const auto perSetStorageBufferInfoCount = std::accumulate(
          storageBufferBindings.begin(), storageBufferBindings.end(), 0, [](uint32_t sum, const auto& elem) {
              return sum + 1;
          });  // TODO: elem->count once array support is added.
        std::vector storageBufferInfos(perSetStorageBufferInfoCount * instanceData.descriptorSets.size(),
                                       VkDescriptorBufferInfo{});

        const auto perSetAccelerationStructureInfoCount = std::accumulate(
          accelerationStructureBindings.begin(),
          accelerationStructureBindings.end(),
          0,
          [](uint32_t sum, const auto& elem) { return sum + 1; });  // TODO: elem->count once array support is added.
        std::vector accelerationStructureInfos(
          perSetAccelerationStructureInfoCount * instanceData.descriptorSets.size(),
          VkWriteDescriptorSetAccelerationStructureKHR{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR});

        // Create descriptor writes. Objects are laid out per set, and then per binding:
        // set<0>{ binding<0>, binding<1>, ..., binding<N> }, set<1>{...}, set<M>{ binding<0>, binding<1>, ..., binding<N>}
        const size_t bindingCount =
          storageImageBindings.size() + storageBufferBindings.size() + accelerationStructureBindings.size();
        std::vector descriptorWrites(bindingCount * instanceData.descriptorSets.size(),
                                     VkWriteDescriptorSet{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET});

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

            offset = 0;

            for (size_t i = 0; i < storageBufferBindings.size(); i++)
            {
                const auto& binding    = storageBufferBindings[i];
                auto&       bufferInfo = storageBufferInfos[set * perSetStorageBufferInfoCount + offset];
                auto&       buffer     = *instanceData.materialInstance->getStorageBufferData(binding.binding);
                bufferInfo.buffer      = buffer.get();
                bufferInfo.offset      = 0;
                bufferInfo.range       = VK_WHOLE_SIZE;

                auto& write            = descriptorWrites[set * bindingCount + binding.binding];
                write.pNext            = nullptr;
                write.dstSet           = instanceData.descriptorSets[set];
                write.dstBinding       = binding.binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = 1;  // TODO: No array support yet.
                write.descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                write.pImageInfo       = nullptr;
                write.pBufferInfo      = storageBufferInfos.data() + set * perSetStorageBufferInfoCount + offset;
                write.pTexelBufferView = nullptr;

                offset++;
            }

            offset = 0;

            for (size_t i = 0; i < accelerationStructureBindings.size(); i++)
            {
                const auto& binding = accelerationStructureBindings[i];
                auto&       asInfo  = accelerationStructureInfos[set * perSetAccelerationStructureInfoCount + offset];
                auto&       as      = *instanceData.materialInstance->getAccelerationStructureData(binding.binding);
                asInfo.accelerationStructureCount = 1;
                asInfo.pAccelerationStructures    = &as.get();

                auto& write            = descriptorWrites[set * bindingCount + binding.binding];
                write.pNext            = &asInfo;
                write.dstSet           = instanceData.descriptorSets[set];
                write.dstBinding       = binding.binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = 1;  // TODO: No array support yet.
                write.descriptorType   = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
                write.pImageInfo       = nullptr;
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

        const auto& mtl                    = instance->getRayTracingMaterial();
        const auto& mtlLayout              = mtl.getLayout();
        const auto  setIndex               = instance->getSetIndex();
        const auto  storageImages          = mtlLayout.getStorageImages(setIndex);
        const auto  storageBuffers         = mtlLayout.getStorageBuffers(setIndex);
        const auto  accelerationStructures = mtlLayout.getAccelerationStructures(setIndex);

        // Create new instance data.
        auto& instanceData =
          *instanceDataMap.try_emplace(instance.get(), std::make_unique<InstanceData>()).first->second;
        instanceData.materialInstance = instance.get();

        // Store material instance.
        assert(materialInstances.try_emplace(instance->getUuid(), std::move(instance)).second);

        // Create descriptor pool and allocate descriptor sets.
        instanceData.pool = createDescriptorPool(memoryManager->getDevice(), mtlLayout, dataSetCount);
        allocateDescriptorSets(memoryManager->getDevice(), instanceData, dataSetCount);

        updateDescriptorSets(instanceData, *memoryManager, storageImages, storageBuffers, accelerationStructures);
    }


}  // namespace sol
