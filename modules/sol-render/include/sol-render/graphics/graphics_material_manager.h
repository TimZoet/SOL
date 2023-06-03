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
#include "sol-material/graphics/graphics_material_instance.h"
#include "sol-material/graphics/i_graphics_material_manager.h"
#include "sol-memory/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/graphics/fwd.h"

namespace sol
{
    class GraphicsMaterialManager final : public IGraphicsMaterialManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct InstanceData
        {
            /**
             * \brief Material instance.
             */
            GraphicsMaterialInstance* materialInstance = nullptr;

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
        using MaterialMap         = std::unordered_map<uuids::uuid, GraphicsMaterialPtr>;
        using MaterialInstanceMap = std::unordered_map<uuids::uuid, GraphicsMaterialInstancePtr>;
        using InstanceDataMap     = std::unordered_map<const GraphicsMaterialInstance*, InstanceDataPtr>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterialManager();

        explicit GraphicsMaterialManager(MemoryManager& memManager);

        GraphicsMaterialManager(const GraphicsMaterialManager&) = delete;

        GraphicsMaterialManager(GraphicsMaterialManager&&) = delete;

        ~GraphicsMaterialManager() noexcept override;

        GraphicsMaterialManager& operator=(const GraphicsMaterialManager&) = delete;

        GraphicsMaterialManager& operator=(GraphicsMaterialManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the number of data sets that are stored for each material instance.
         * \return Number of sets.
         */
        [[nodiscard]] size_t getDataSetCount() const noexcept;

        [[nodiscard]] const InstanceDataMap& getInstanceData() const noexcept;

        VulkanGraphicsPipeline& getPipeline(const GraphicsMaterial& material,
                                            const VulkanRenderPass& renderPass) const override;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setDataSetCount(size_t count);

        ////////////////////////////////////////////////////////////////
        // Materials.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Add a new material.
         * \tparam T Type derived from GraphicsMaterial.
         * \param material Material to add.
         * \return GraphicsMaterial.
         */
        template<std::derived_from<GraphicsMaterial> T>
        T& addMaterial(std::unique_ptr<T> material)
        {
            auto& mtl = *material;
            addMaterialImpl(std::move(material));
            return mtl;
        }

        /**
         * \brief Add a new material instance.
         * \tparam T Type derived from GraphicsMaterialInstance.
         * \param material Material. Should be owned by this manager.
         * \param materialInstance Material instance to add.
         * \return GraphicsMaterialInstance.
         */
        template<std::derived_from<GraphicsMaterialInstance> T>
        T& addMaterialInstance(GraphicsMaterial& material, std::unique_ptr<T> materialInstance)
        {
            auto& mtl = *materialInstance;
            addMaterialInstanceImpl(material, std::move(materialInstance));
            return mtl;
        }

        /**
         * \brief Create a new pipeline for the given material with the settings and renderpass, if one does not exist yet.
         * \param material GraphicsMaterial.
         * \param renderPass RenderPass.
         * \return True if a new pipeline was created, false if one already existed.
         */
        bool createPipeline(const GraphicsMaterial& material, VulkanRenderPass& renderPass) const override;

        void bindDescriptorSets(std::span<const GraphicsMaterialInstance* const> instances,
                                VkCommandBuffer                                  commandBuffer,
                                const VulkanGraphicsPipeline&                    pipeline,
                                size_t                                           index) const override;

        void destroyMaterial(GraphicsMaterial& material);

        void destroyMaterialInstance(GraphicsMaterialInstance& materialInstance);

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Update the uniform buffers of all modified material instances.
         * \param index Data set index.
         */
        void updateUniformBuffers(uint32_t index) override;

    private:
        void addMaterialImpl(GraphicsMaterialPtr material);

        void addMaterialInstanceImpl(GraphicsMaterial& material, GraphicsMaterialInstancePtr instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        MaterialMap materials;

        MaterialInstanceMap materialInstances;

        GraphicsPipelineCachePtr pipelineCache;

        std::unique_ptr<UniformBufferManager> uniformBufferManager;

        InstanceDataMap instanceDataMap;

        /**
         * \brief Number of data sets stored per material instance.
         */
        size_t dataSetCount = 0;
    };
}  // namespace sol
