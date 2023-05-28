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

        RayTracingMaterial() = default;

        RayTracingMaterial(VulkanShaderModule* raygenModule,
                           VulkanShaderModule* missModule,
                           VulkanShaderModule* closestHitModule,
                           VulkanShaderModule* anyHitModule,
                           VulkanShaderModule* intersectionModule);

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

        [[nodiscard]] bool hasRaygenShader() const noexcept;

        [[nodiscard]] VulkanShaderModule& getRaygenShader() noexcept;

        [[nodiscard]] const VulkanShaderModule& getRaygenShader() const noexcept;

        [[nodiscard]] bool hasMissShader() const noexcept;

        [[nodiscard]] VulkanShaderModule& getMissShader() noexcept;

        [[nodiscard]] const VulkanShaderModule& getMissShader() const noexcept;

        [[nodiscard]] bool hasClosestHitShader() const noexcept;

        [[nodiscard]] VulkanShaderModule& getClosestHitShader() noexcept;

        [[nodiscard]] const VulkanShaderModule& getClosestHitShader() const noexcept;

        [[nodiscard]] bool hasAnyHitShader() const noexcept;

        [[nodiscard]] VulkanShaderModule& getAnyHitShader() noexcept;

        [[nodiscard]] const VulkanShaderModule& getAnyHitShader() const noexcept;

        [[nodiscard]] bool hasIntersectionShader() const noexcept;

        [[nodiscard]] VulkanShaderModule& getIntersectionShader() noexcept;

        [[nodiscard]] const VulkanShaderModule& getIntersectionShader() const noexcept;

        [[nodiscard]] const RayTracingMaterialLayout& getLayout() const noexcept;

        [[nodiscard]] const std::vector<RayTracingMaterialInstance*>& getInstances() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IRayTracingMaterialManager& manager);

    private:
        void addInstance(RayTracingMaterialInstance& instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IRayTracingMaterialManager* materialManager = nullptr;

        VulkanShaderModule* raygenShader = nullptr;

        VulkanShaderModule* missShader = nullptr;

        VulkanShaderModule* closestHitShader = nullptr;

        VulkanShaderModule* anyHitShader = nullptr;

        VulkanShaderModule* intersectionShader = nullptr;

        std::vector<RayTracingMaterialInstance*> instances;

    protected:
        RayTracingMaterialLayout layout;
    };
}  // namespace sol
