#include "sol-render/forward/forward_material_manager.h"

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
#include "sol-core/vulkan_sampler.h"
#include "sol-error/sol_error.h"
#include "sol-error/vulkan_error_handler.h"
#include "sol-material/forward/forward_material.h"
#include "sol-material/forward/forward_material_instance.h"
#include "sol-memory/memory_manager.h"
#include "sol-texture/image2d.h"
#include "sol-texture/texture2d.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/common/render_settings.h"

namespace
{
    [[nodiscard]] sol::VulkanDescriptorPoolPtr
      createDescriptorPool(sol::VulkanDevice& device, const sol::ForwardMaterialLayout& layout, const size_t count)
    {
        sol::VulkanDescriptorPool::Settings poolSettings;
        poolSettings.device  = device;
        poolSettings.maxSets = static_cast<uint32_t>(count);

        // Uniform buffers.
        if (layout.getUniformBufferCount() > 0)
        {
            poolSettings.poolSizes.emplace_back(
              VkDescriptorPoolSize{.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                   .descriptorCount = static_cast<uint32_t>(count * layout.getUniformBufferCount())});
        }

        // Combined image samplers.
        if (layout.getSamplerCount() > 0)
        {
            poolSettings.poolSizes.emplace_back(
              VkDescriptorPoolSize{.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                   .descriptorCount = static_cast<uint32_t>(count * layout.getSamplerCount())});
        }

        return sol::VulkanDescriptorPool::create(std::move(poolSettings));
    }

    void allocateDescriptorSets(const sol::VulkanDevice&                   device,
                                sol::ForwardMaterialManager::InstanceData& instanceData,
                                const size_t                               count)
    {
        const auto& materialInstance = *instanceData.materialInstance;
        const auto& material         = materialInstance.getForwardMaterial();
        const auto& mtlLayout        = material.getLayout();
        const auto  setLayout = mtlLayout.getFinalizedDescriptorSetLayouts()[materialInstance.getSetIndex()]->get();

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

    void createNoneUniformBuffers(
      sol::ForwardMaterialManager::InstanceData&                                               instanceData,
      sol::UniformBufferManager&                                                               manager,
      const std::vector<std::pair<sol::ForwardMaterialLayout::UniformBufferBinding*, size_t>>& uniformBufferBindings)
    {
        for (const auto& [ubb, index] : uniformBufferBindings)
        {
            // Create a uniform buffer with a single slot.
            auto* uniformBuffer = manager.create(instanceData.materialInstance->getMaterial(),
                                                 instanceData.materialInstance->getSetIndex(),
                                                 ubb->binding,
                                                 sol::ForwardMaterialLayout::SharingMethod::None,
                                                 1,
                                                 ubb->size * ubb->count);

            // Acquire free slot.
            const auto [slot, offset] = uniformBuffer->getFreeSlot();

            instanceData.uniformBuffers[index] = {.uniformBuffer = uniformBuffer, .slot = slot, .offset = offset};
        }
    }

    void createInstanceUniformBuffers(
      sol::ForwardMaterialManager::InstanceData&                                               instanceData,
      sol::UniformBufferManager&                                                               manager,
      const std::vector<std::pair<sol::ForwardMaterialLayout::UniformBufferBinding*, size_t>>& uniformBufferBindings)
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
                                             sol::ForwardMaterialLayout::SharingMethod::Instance,
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
      sol::ForwardMaterialManager::InstanceData&                                               instanceData,
      sol::UniformBufferManager&                                                               manager,
      const std::vector<std::pair<sol::ForwardMaterialLayout::UniformBufferBinding*, size_t>>& uniformBufferBindings)
    {
        for (const auto& [ubb, index] : uniformBufferBindings)
        {
            // Query for an existing uniform buffer with a free slot, or create a new one.
            auto* uniformBuffer = manager.getOrCreate(instanceData.materialInstance->getMaterial(),
                                                      instanceData.materialInstance->getSetIndex(),
                                                      ubb->binding,
                                                      sol::ForwardMaterialLayout::SharingMethod::Binding,
                                                      ubb->sharing.count,
                                                      ubb->size * ubb->count);

            // Acquire free slot.
            const auto [slot, offset] = uniformBuffer->getFreeSlot();

            instanceData.uniformBuffers[index] = {.uniformBuffer = uniformBuffer, .slot = slot, .offset = offset};
        }
    }

    void createInstanceAndBindingUniformBuffers(
      sol::ForwardMaterialManager::InstanceData&                                               instanceData,
      sol::UniformBufferManager&                                                               manager,
      const std::vector<std::pair<sol::ForwardMaterialLayout::UniformBufferBinding*, size_t>>& uniformBufferBindings)
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
                                                  sol::ForwardMaterialLayout::SharingMethod::InstanceAndBinding,
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

    void updateDescriptorSets(
      const sol::ForwardMaterialManager::InstanceData&                                     instanceData,
      sol::MemoryManager&                                                                  manager,
      const std::pair<const sol::ForwardMaterialLayout::UniformBufferBindingPtr*, size_t>& uniformBufferBindings,
      const std::pair<const sol::ForwardMaterialLayout::SamplerBindingPtr*, size_t>&       samplerBindings)
    {
        const auto perSetBufferInfoCount =
          std::accumulate(uniformBufferBindings.first,
                          uniformBufferBindings.first + uniformBufferBindings.second,
                          0,
                          [](uint32_t sum, const auto& elem) { return sum + elem->count; });
        const auto perSetImageInfoCount = std::accumulate(
          samplerBindings.first, samplerBindings.first + samplerBindings.second, 0, [](uint32_t sum, const auto& elem) {
              return sum + elem->count;
          });
        std::vector bufferInfos(perSetBufferInfoCount * instanceData.descriptorSets.size(), VkDescriptorBufferInfo{});
        std::vector imageInfos(perSetImageInfoCount * instanceData.descriptorSets.size(), VkDescriptorImageInfo{});

        // Create descriptor writes. Objects are laid out per set, and then per binding:
        // set<0>{ binding<0>, binding<1>, ..., binding<N> }, set<1>{...}, set<M>{ binding<0>, binding<1>, ..., binding<N>}
        const size_t bindingCount = uniformBufferBindings.second + samplerBindings.second;
        std::vector  descriptorWrites(bindingCount * instanceData.descriptorSets.size(), VkWriteDescriptorSet{});

        for (size_t set = 0; set < instanceData.descriptorSets.size(); set++)
        {
            size_t bufferInfoOffset = 0;

            for (size_t i = 0; i < uniformBufferBindings.second; i++)
            {
                const auto& ubb                           = uniformBufferBindings.first[i];
                const auto& [uniformBuffer, slot, offset] = instanceData.uniformBuffers[i];

                for (size_t elem = 0; elem < ubb->count; elem++)
                {
                    auto& bufferInfo  = bufferInfos[set * perSetBufferInfoCount + bufferInfoOffset + elem];
                    bufferInfo.buffer = uniformBuffer->getBuffer(set).get();
                    bufferInfo.offset = offset + elem * ubb->size;
                    bufferInfo.range  = ubb->size;
                }

                auto& write            = descriptorWrites[set * bindingCount + ubb->binding];
                write.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.pNext            = nullptr;
                write.dstSet           = instanceData.descriptorSets[set];
                write.dstBinding       = ubb->binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = ubb->count;
                write.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.pImageInfo       = nullptr;
                write.pBufferInfo      = bufferInfos.data() + set * perSetBufferInfoCount + bufferInfoOffset;
                write.pTexelBufferView = nullptr;

                bufferInfoOffset += ubb->count;
            }


            size_t imageInfoOffset = 0;

            for (size_t i = 0; i < samplerBindings.second; i++)
            {
                const auto& sampler = samplerBindings.first[i];
                for (size_t elem = 0; elem < sampler->count; elem++)
                {
                    auto& imageInfo       = imageInfos[set * perSetImageInfoCount + imageInfoOffset + elem];
                    auto& texture         = *instanceData.materialInstance->getTextureData(sampler->binding);
                    imageInfo.sampler     = texture.getSampler().get();
                    imageInfo.imageView   = texture.getImageView()->get();
                    imageInfo.imageLayout = texture.getImage()->getTargetLayout();
                }

                auto& write            = descriptorWrites[set * bindingCount + sampler->binding];
                write.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.pNext            = nullptr;
                write.dstSet           = instanceData.descriptorSets[set];
                write.dstBinding       = sampler->binding;
                write.dstArrayElement  = 0;
                write.descriptorCount  = sampler->count;
                write.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write.pImageInfo       = imageInfos.data() + set * perSetImageInfoCount + imageInfoOffset;
                write.pBufferInfo      = nullptr;
                write.pTexelBufferView = nullptr;

                imageInfoOffset += sampler->count;
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

        const auto& mtl                              = instance->getForwardMaterial();
        const auto& mtlLayout                        = mtl.getLayout();
        const auto  setIndex                         = instance->getSetIndex();
        const auto [uniformBufferBindings, ubbCount] = mtlLayout.getUniformBuffers(setIndex);
        const auto [samplerBindings, sCount]         = mtlLayout.getSamplers(setIndex);

        // Create new instance data.
        auto& instanceData =
          *instanceDataMap.try_emplace(instance.get(), std::make_unique<InstanceData>()).first->second;
        instanceData.materialInstance = instance.get();

        // Store material instance.
        assert(materialInstances.try_emplace(instance->getUuid(), std::move(instance)).second);

        // Create descriptor pool and allocate descriptor sets.
        instanceData.pool = createDescriptorPool(memoryManager->getDevice(), mtlLayout, dataSetCount);
        allocateDescriptorSets(memoryManager->getDevice(), instanceData, dataSetCount);

        // Allocate a reference to a uniform buffer for each binding.
        instanceData.uniformBuffers.resize(ubbCount);

        // Initially mark all bindings in all data sets as stale and initialize checksums to 0.
        instanceData.stale.resize(ubbCount * dataSetCount, 1);
        instanceData.checksum.resize(ubbCount, 0);

        // Group uniformBufferBindings by SharingMode.
        using list_t = std::vector<std::pair<ForwardMaterialLayout::UniformBufferBinding*, size_t>>;
        list_t ubbsNone;
        list_t ubbsInstance;
        list_t ubbsBinding;
        list_t ubbsBoth;
        for (size_t i = 0; i < ubbCount; i++)
        {
            switch (const auto& ubb = uniformBufferBindings[i]; ubb->sharing.method)
            {
            case ForwardMaterialLayout::SharingMethod::None: ubbsNone.emplace_back(ubb.get(), i); break;
            case ForwardMaterialLayout::SharingMethod::Instance: ubbsInstance.emplace_back(ubb.get(), i); break;
            case ForwardMaterialLayout::SharingMethod::Binding: ubbsBinding.emplace_back(ubb.get(), i); break;
            case ForwardMaterialLayout::SharingMethod::InstanceAndBinding: ubbsBoth.emplace_back(ubb.get(), i); break;
            }
        }

        // Create UniformBuffers.
        if (!ubbsNone.empty()) createNoneUniformBuffers(instanceData, *uniformBufferManager, ubbsNone);
        if (!ubbsInstance.empty()) createInstanceUniformBuffers(instanceData, *uniformBufferManager, ubbsInstance);
        if (!ubbsBinding.empty()) createBindingUniformBuffers(instanceData, *uniformBufferManager, ubbsBinding);
        if (!ubbsBoth.empty()) createInstanceAndBindingUniformBuffers(instanceData, *uniformBufferManager, ubbsBoth);

        updateDescriptorSets(
          instanceData, *memoryManager, {uniformBufferBindings, ubbCount}, {samplerBindings, sCount});
    }

    VulkanGraphicsPipelinePtr ForwardMaterialManager::createPipelineImpl(const ForwardMaterial& material,
                                                                         RenderSettings&        renderSettings,
                                                                         VulkanRenderPass&      renderPass)
    {
        const auto* meshLayout = material.getMeshLayout();
        if (!meshLayout) throw SolError("Cannot create pipeline: material has no mesh layout.");

        // TODO: To what extent are the RenderSettings needed here? Things like culling should perhaps be put in the ForwardMaterial class instead.
        VulkanGraphicsPipeline::Settings pipelineSettings;
        pipelineSettings.renderPass           = renderPass;
        pipelineSettings.vertexShader         = *material.getVertexShader();
        pipelineSettings.fragmentShader       = *material.getFragmentShader();
        pipelineSettings.vertexAttributes     = meshLayout->getAttributeDescriptions();
        pipelineSettings.vertexBindings       = meshLayout->getBindingDescriptions();
        pipelineSettings.descriptorSetLayouts = material.getLayout().getFinalizedDescriptorSetLayouts();
        pipelineSettings.pushConstants        = material.getLayout().getFinalizedPushConstants();
        pipelineSettings.colorBlending        = material.getLayout().getColorBlending();

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
            const auto [buffers, count]  = layout.getUniformBuffers(materialInstance.getSetIndex());

            for (size_t i = 0; i < count; i++)
            {
                const auto& ubb = *buffers[i];

                // Skip if data is never updated.
                if (ubb.usage.updateFrequency == ForwardMaterialLayout::UpdateFrequency::Never) continue;

                bool     stale = false;
                uint32_t checksum;

                // Determine if data is stale using the required update detection method.
                switch (ubb.usage.updateDetection)
                {
                case ForwardMaterialLayout::UpdateDetection::Manual:
                    // Query material instance to see if data was updated.
                    stale = materialInstance.isUniformBufferStale(ubb.binding);
                    break;
                case ForwardMaterialLayout::UpdateDetection::Automatic:
                    // Calculate checksum of data and compare to previously recorded checksum.
                    checksum = crc32(static_cast<const uint8_t*>(materialInstance.getUniformBufferData(ubb.binding)),
                                     ubb.size * ubb.count);
                    if (checksum != instanceData->checksum[i])
                    {
                        stale                     = true;
                        instanceData->checksum[i] = checksum;
                    }
                    break;
                case ForwardMaterialLayout::UpdateDetection::Always:
                    // Always update data.
                    stale = true;
                    break;
                }

                // Mark binding as stale for all data sets.
                if (stale)
                {
                    for (size_t set = 0; set < dataSetCount; set++) instanceData->stale[set * count + i] = true;
                }
            }
        }

        for (const auto& instanceData : instanceDataMap | std::views::values)
        {
            const auto& materialInstance = *instanceData->materialInstance;
            const auto& material         = materialInstance.getForwardMaterial();
            const auto& layout           = material.getLayout();
            const auto [buffers, count]  = layout.getUniformBuffers(materialInstance.getSetIndex());

            for (size_t i = 0; i < count; i++)
            {
                const auto& ubb  = *buffers[i];
                const auto* data = materialInstance.getUniformBufferData(ubb.binding);

                if (instanceData->stale[index * count + i])
                {
                    const auto& tuple = instanceData->uniformBuffers[i];
                    tuple.uniformBuffer->getBuffer(index).setData(data, ubb.size * ubb.count, tuple.offset);
                    instanceData->stale[index * count + i] = false;
                }
            }
        }

        uniformBufferManager->unmapAll(index);
        uniformBufferManager->flushAll(index);
    }
}  // namespace sol