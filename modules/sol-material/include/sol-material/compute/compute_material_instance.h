#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-texture/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"
#include "sol-material/material_instance.h"

namespace sol
{
    class ComputeMaterialInstance : public MaterialInstance
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeMaterialInstance();

        ComputeMaterialInstance(const ComputeMaterialInstance&) = delete;

        ComputeMaterialInstance(ComputeMaterialInstance&&) = delete;

        ~ComputeMaterialInstance() override;

        ComputeMaterialInstance& operator=(const ComputeMaterialInstance&) = delete;

        ComputeMaterialInstance& operator=(ComputeMaterialInstance&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the IComputeMaterialManager.
         * \return IComputeMaterialManager.
         */
        [[nodiscard]] IComputeMaterialManager& getMaterialManager() noexcept;

        /**
         * \brief Get the IComputeMaterialManager.
         * \return IComputeMaterialManager.
         */
        [[nodiscard]] const IComputeMaterialManager& getMaterialManager() const noexcept;

        [[nodiscard]] Material& getMaterial() noexcept override;

        [[nodiscard]] const Material& getMaterial() const noexcept override;

        [[nodiscard]] ComputeMaterial& getComputeMaterial() noexcept;

        [[nodiscard]] const ComputeMaterial& getComputeMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IComputeMaterialManager& manager);

        void setMaterial(ComputeMaterial& fwdMaterial);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IComputeMaterialManager* materialManager = nullptr;

        ComputeMaterial* material = nullptr;
    };
}  // namespace sol
