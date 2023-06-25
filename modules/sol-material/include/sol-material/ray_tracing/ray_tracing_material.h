#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_ray_tracing_pipeline.h"
#include "sol-material/fwd.h"
#include "sol-material/material.h"
#include "sol-material/ray_tracing/ray_tracing_material_layout.h"

namespace sol
{
    class RayTracingMaterial : public Material
    {
    public:
        friend class RayTracingMaterialInstance;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingMaterial() = delete;

        explicit RayTracingMaterial(VulkanDevice& device);

        RayTracingMaterial(const RayTracingMaterial&) = delete;

        RayTracingMaterial(RayTracingMaterial&&) = delete;

        ~RayTracingMaterial() override;

        RayTracingMaterial& operator=(const RayTracingMaterial&) = delete;

        RayTracingMaterial& operator=(RayTracingMaterial&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the IRayTracingMaterialManager.
         * \return IRayTracingMaterialManager.
         */
        [[nodiscard]] IRayTracingMaterialManager& getMaterialManager() noexcept;

        /**
         * \brief Get the IRayTracingMaterialManager.
         * \return IRayTracingMaterialManager.
         */
        [[nodiscard]] const IRayTracingMaterialManager& getMaterialManager() const noexcept;

        [[nodiscard]] VulkanShaderModule* getRaygenShader() const noexcept;

        [[nodiscard]] const std::vector<VulkanShaderModule*>& getMissShaders() const noexcept;

        [[nodiscard]] const std::vector<VulkanShaderModule*>& getClosestHitShaders() const noexcept;

        [[nodiscard]] const std::vector<VulkanShaderModule*>& getAnyHitShaders() const noexcept;

        [[nodiscard]] const std::vector<VulkanShaderModule*>& getIntersectionShaders() const noexcept;

        [[nodiscard]] const std::vector<VulkanShaderModule*>& getCallableShaders() const noexcept;

        [[nodiscard]] const std::vector<VulkanRayTracingPipeline::HitShaderGroup>& getHitGroups() const noexcept;

        [[nodiscard]] MaterialLayout& getLayout() noexcept override;

        [[nodiscard]] const MaterialLayout& getLayout() const noexcept override;

        [[nodiscard]] RayTracingMaterialLayout& getRayTracingLayout() noexcept;

        [[nodiscard]] const RayTracingMaterialLayout& getRayTracingLayout() const noexcept;

        [[nodiscard]] const std::vector<RayTracingMaterialInstance*>& getInstances() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IRayTracingMaterialManager& manager);

        void setRaygenShader(VulkanShaderModule& shader) noexcept;

        void addMissShader(VulkanShaderModule& shader) noexcept;

        void addClosestHitShader(VulkanShaderModule& shader) noexcept;

        void addAnyHitShader(VulkanShaderModule& shader) noexcept;

        void addIntersectionShader(VulkanShaderModule& shader) noexcept;

        void addCallableShader(VulkanShaderModule& shader) noexcept;

        void addHitGroup(std::optional<uint32_t> closest,
                         std::optional<uint32_t> any,
                         std::optional<uint32_t> intersection);

    private:
        void addInstance(RayTracingMaterialInstance& instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IRayTracingMaterialManager* materialManager = nullptr;

        VulkanShaderModule* raygenShader = nullptr;

        std::vector<VulkanShaderModule*> missShaders;

        std::vector<VulkanShaderModule*> closestHitShaders;

        std::vector<VulkanShaderModule*> anyHitShaders;

        std::vector<VulkanShaderModule*> intersectionShaders;

        std::vector<VulkanShaderModule*> callableShaders;

        std::vector<VulkanRayTracingPipeline::HitShaderGroup> hitGroups;

        std::vector<RayTracingMaterialInstance*> instances;

    protected:
        RayTracingMaterialLayout layout;
    };
}  // namespace sol
