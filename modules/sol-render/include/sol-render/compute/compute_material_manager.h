#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid.h>
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/fwd.h"
#include "sol-material/compute/i_compute_material_manager.h"
#include "sol-memory/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/fwd.h"
#include "sol-render/compute/fwd.h"

namespace sol
{
    // TODO: This class does not implement any virtual methods of the base yet, because there are none.
    // Also, the ComputeRenderer is doing dynamic_casts because of that.
    class ComputeMaterialManager final : public IComputeMaterialManager
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
            ComputeMaterialInstance* materialInstance = nullptr;

            /**
             * \brief Descriptor pool.
             */
            VulkanDescriptorPoolPtr pool;

            /**
             * \brief List of descriptor sets, one for each data set.
             */
            std::vector<VkDescriptorSet> descriptorSets;
        };

        using InstanceDataPtr     = std::unique_ptr<InstanceData>;
        using MaterialMap         = std::unordered_map<uuids::uuid, ComputeMaterialPtr>;
        using MaterialInstanceMap = std::unordered_map<uuids::uuid, ComputeMaterialInstancePtr>;
        using InstanceDataMap     = std::unordered_map<const ComputeMaterialInstance*, InstanceDataPtr>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeMaterialManager() = delete;

        explicit ComputeMaterialManager(MemoryManager& memManager);

        ComputeMaterialManager(const ComputeMaterialManager&) = delete;

        ComputeMaterialManager(ComputeMaterialManager&&) = delete;

        ~ComputeMaterialManager() noexcept override;

        ComputeMaterialManager& operator=(const ComputeMaterialManager&) = delete;

        ComputeMaterialManager& operator=(ComputeMaterialManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the number of data sets that are stored for each material instance.
         * \return Number of sets.
         */
        [[nodiscard]] size_t getDataSetCount() const noexcept;

        [[nodiscard]] const InstanceDataMap& getInstanceData() const noexcept;

        [[nodiscard]] VulkanComputePipeline& getPipeline(const ComputeMaterial& material) const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setDataSetCount(size_t count);

        ////////////////////////////////////////////////////////////////
        // Materials.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Add a new material.
         * \tparam T Type derived from ComputeMaterial.
         * \param material Material to add.
         * \return ComputeMaterial.
         */
        template<std::derived_from<ComputeMaterial> T>
        T& addMaterial(std::unique_ptr<T> material)
        {
            auto& mtl = *material;
            addMaterialImpl(std::move(material));
            return mtl;
        }

        /**
         * \brief Add a new material instance.
         * \tparam T Type derived from ComputeMaterialInstance.
         * \param material Material. Should be owned by this manager.
         * \param materialInstance Material instance to add.
         * \return ComputeMaterialInstance.
         */
        template<std::derived_from<ComputeMaterialInstance> T>
        T& addMaterialInstance(ComputeMaterial& material, std::unique_ptr<T> materialInstance)
        {
            auto& mtl = *materialInstance;
            addMaterialInstanceImpl(material, std::move(materialInstance));
            return mtl;
        }

        void destroyMaterial(ComputeMaterial& material);

        void destroyMaterialInstance(ComputeMaterialInstance& materialInstance);

        bool createPipeline(const ComputeMaterial& material) const;

    private:
        void addMaterialImpl(ComputeMaterialPtr material);

        void addMaterialInstanceImpl(ComputeMaterial& material, ComputeMaterialInstancePtr instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        MaterialMap materials;

        MaterialInstanceMap materialInstances;

        ComputePipelineCachePtr pipelineCache;

        InstanceDataMap instanceDataMap;

        /**
         * \brief Number of data sets stored per material instance.
         */
        size_t dataSetCount = 0;
    };
}  // namespace sol
