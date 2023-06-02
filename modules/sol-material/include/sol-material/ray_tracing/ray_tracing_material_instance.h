#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-texture/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"
#include "sol-material/material_instance.h"

namespace sol
{
    class RayTracingMaterialInstance : public MaterialInstance
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingMaterialInstance();

        RayTracingMaterialInstance(const RayTracingMaterialInstance&) = delete;

        RayTracingMaterialInstance(RayTracingMaterialInstance&&) = delete;

        ~RayTracingMaterialInstance() override;

        RayTracingMaterialInstance& operator=(const RayTracingMaterialInstance&) = delete;

        RayTracingMaterialInstance& operator=(RayTracingMaterialInstance&&) = delete;

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

        [[nodiscard]] Material& getMaterial() noexcept override;

        [[nodiscard]] const Material& getMaterial() const noexcept override;

        [[nodiscard]] RayTracingMaterial& getRayTracingMaterial() noexcept;

        [[nodiscard]] const RayTracingMaterial& getRayTracingMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IRayTracingMaterialManager& manager);

        void setMaterial(RayTracingMaterial& rtMaterial);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IRayTracingMaterialManager* materialManager = nullptr;

        RayTracingMaterial* material = nullptr;
    };
}  // namespace sol
