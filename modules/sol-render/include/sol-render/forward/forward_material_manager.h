#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/fwd.h"
#include "sol-material/common/uniform_buffer_manager.h"
#include "sol-material/forward/forward_material_instance.h"
#include "sol-material/forward/i_forward_material_manager.h"
#include "sol-memory/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/fwd.h"
#include "sol-render/forward/fwd.h"

namespace sol
{
    // TODO: This class does not implement any virtual methods of the base yet, because there are none.
    // Also, the ForwardRenderer is doing dynamic_casts because of that.
    class ForwardMaterialManager : public IForwardMaterialManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Pipeline
        {
            VulkanGraphicsPipelinePtr pipeline;

            RenderSettings* renderSettings = nullptr;

            VulkanRenderPass* renderPass = nullptr;
        };

        struct InstanceData
        {
            /**
             * \brief Material instance.
             */
            ForwardMaterialInstance* materialInstance = nullptr;

            /**
             * \brief Descriptor pool.
             */
            VulkanDescriptorPoolPtr pool;

            /**
             * \brief List of descriptor sets, one for each data set.
             */
            std::vector<VkDescriptorSet> descriptorSets;

            /**
             * \brief References to uniform buffers, one for each uniform binding.
             */
            std::vector<UniformBufferManager::UniformBufferReference> uniformBuffers;

            /**
             * \brief Flags indicating data is stale. One for each uniform binding, repeated for each data set.
             */
            std::vector<uint8_t> stale;

            /**
             * \brief Checksum of data currently in buffers. One for each uniform binding.
             */
            std::vector<uint32_t> checksum;
        };

        using InstanceDataPtr     = std::unique_ptr<InstanceData>;
        using MaterialMap         = std::unordered_map<uuids::uuid, ForwardMaterialPtr>;
        using MaterialInstanceMap = std::unordered_map<uuids::uuid, ForwardMaterialInstancePtr>;
        using InstanceDataMap     = std::unordered_map<const ForwardMaterialInstance*, InstanceDataPtr>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ForwardMaterialManager();

        explicit ForwardMaterialManager(MemoryManager& memManager);

        ForwardMaterialManager(const ForwardMaterialManager&) = delete;

        ForwardMaterialManager(ForwardMaterialManager&&) = delete;

        ~ForwardMaterialManager() noexcept override;

        ForwardMaterialManager& operator=(const ForwardMaterialManager&) = delete;

        ForwardMaterialManager& operator=(ForwardMaterialManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the number of data sets that are stored for each material instance.
         * \return Number of sets.
         */
        [[nodiscard]] size_t getDataSetCount() const noexcept;

        [[nodiscard]] const InstanceDataMap& getInstanceData() const noexcept;

        VulkanGraphicsPipeline& getPipeline(const ForwardMaterial&  material,
                                            const RenderSettings&   renderSettings,
                                            const VulkanRenderPass& renderPass) const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setDataSetCount(size_t count);

        ////////////////////////////////////////////////////////////////
        // Materials.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Add a new material.
         * \tparam T Type derived from ForwardMaterial.
         * \param material Material to add.
         * \return ForwardMaterial.
         */
        template<std::derived_from<ForwardMaterial> T>
        T& addMaterial(std::unique_ptr<T> material)
        {
            auto& mtl = *material;
            addMaterialImpl(std::move(material));
            return mtl;
        }

        /**
         * \brief Add a new material instance.
         * \tparam T Type derived from ForwardMaterialInstance.
         * \param material Material. Should be owned by this manager.
         * \param materialInstance Material instance to add.
         * \return ForwardMaterialInstance.
         */
        template<std::derived_from<ForwardMaterialInstance> T>
        T& addMaterialInstance(ForwardMaterial& material, std::unique_ptr<T> materialInstance)
        {
            auto& mtl = *materialInstance;
            addMaterialInstanceImpl(material, std::move(materialInstance));
            return mtl;
        }

        /**
         * \brief Create a new pipeline for the given material with the settings and renderpass, if one does not exist yet.
         * \param material ForwardMaterial.
         * \param renderSettings RenderSettings.
         * \param renderPass RenderPass.
         * \return True if a new pipeline was created, false if one already existed.
         */
        bool createPipeline(const ForwardMaterial& material,
                            RenderSettings&        renderSettings,
                            VulkanRenderPass&      renderPass) const;

        void destroyMaterial(ForwardMaterial& material);

        void destroyMaterialInstance(ForwardMaterialInstance& materialInstance);

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Update the uniform buffers of all modified material instances.
         * \param index Data set index.
         */
        void updateUniformBuffers(uint32_t index);

    private:
        void addMaterialImpl(ForwardMaterialPtr material);

        void addMaterialInstanceImpl(ForwardMaterial& material, ForwardMaterialInstancePtr instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        MaterialMap materials;

        MaterialInstanceMap materialInstances;

        ForwardPipelineCachePtr pipelineCache;

        std::unique_ptr<UniformBufferManager> uniformBufferManager;

        InstanceDataMap instanceDataMap;

        /**
         * \brief Number of data sets stored per material instance.
         */
        size_t dataSetCount = 0;
    };
}  // namespace sol
