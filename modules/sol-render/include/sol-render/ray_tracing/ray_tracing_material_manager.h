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
#include "sol-material/ray_tracing/i_ray_tracing_material_manager.h"
#include "sol-memory/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/fwd.h"
#include "sol-render/ray_tracing/fwd.h"

namespace sol
{
    // TODO: This class does not implement any virtual methods of the base yet, because there are none.
    // Also, the RayTracingRenderer is doing dynamic_casts because of that.
    class RayTracingMaterialManager final : public IRayTracingMaterialManager
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
            RayTracingMaterialInstance* materialInstance = nullptr;

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
        using MaterialMap         = std::unordered_map<uuids::uuid, RayTracingMaterialPtr>;
        using MaterialInstanceMap = std::unordered_map<uuids::uuid, RayTracingMaterialInstancePtr>;
        using InstanceDataMap     = std::unordered_map<const RayTracingMaterialInstance*, InstanceDataPtr>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingMaterialManager() = delete;

        explicit RayTracingMaterialManager(MemoryManager& memManager);

        RayTracingMaterialManager(const RayTracingMaterialManager&) = delete;

        RayTracingMaterialManager(RayTracingMaterialManager&&) = delete;

        ~RayTracingMaterialManager() noexcept override;

        RayTracingMaterialManager& operator=(const RayTracingMaterialManager&) = delete;

        RayTracingMaterialManager& operator=(RayTracingMaterialManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the number of data sets that are stored for each material instance.
         * \return Number of sets.
         */
        [[nodiscard]] size_t getDataSetCount() const noexcept;

        [[nodiscard]] const InstanceDataMap& getInstanceData() const noexcept;

        [[nodiscard]] VulkanRayTracingPipeline& getPipeline(const RayTracingMaterial& material) const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setDataSetCount(size_t count);

        ////////////////////////////////////////////////////////////////
        // Materials.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Add a new material.
         * \tparam T Type derived from RayTracingMaterial.
         * \param material Material to add.
         * \return RayTracingMaterial.
         */
        template<std::derived_from<RayTracingMaterial> T>
        T& addMaterial(std::unique_ptr<T> material)
        {
            auto& mtl = *material;
            addMaterialImpl(std::move(material));
            return mtl;
        }

        /**
         * \brief Add a new material instance.
         * \tparam T Type derived from RayTracingMaterialInstance.
         * \param material Material. Should be owned by this manager.
         * \param materialInstance Material instance to add.
         * \return RayTracingMaterialInstance.
         */
        template<std::derived_from<RayTracingMaterialInstance> T>
        T& addMaterialInstance(RayTracingMaterial& material, std::unique_ptr<T> materialInstance)
        {
            auto& mtl = *materialInstance;
            addMaterialInstanceImpl(material, std::move(materialInstance));
            return mtl;
        }

        void destroyMaterial(RayTracingMaterial& material);

        void destroyMaterialInstance(RayTracingMaterialInstance& materialInstance);

        bool createPipeline(const RayTracingMaterial& material) const;

    private:
        void addMaterialImpl(RayTracingMaterialPtr material);

        void addMaterialInstanceImpl(RayTracingMaterial& material, RayTracingMaterialInstancePtr instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        MaterialMap materials;

        MaterialInstanceMap materialInstances;

        RayTracingPipelineCachePtr pipelineCache;

        InstanceDataMap instanceDataMap;

        /**
         * \brief Number of data sets stored per material instance.
         */
        size_t dataSetCount = 0;
    };
}  // namespace sol
