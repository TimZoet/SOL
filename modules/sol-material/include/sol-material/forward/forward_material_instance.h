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
    class ForwardMaterialInstance : public MaterialInstance
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ForwardMaterialInstance();

        ForwardMaterialInstance(const ForwardMaterialInstance&) = delete;

        ForwardMaterialInstance(ForwardMaterialInstance&&) = delete;

        ~ForwardMaterialInstance() override;

        ForwardMaterialInstance& operator=(const ForwardMaterialInstance&) = delete;

        ForwardMaterialInstance& operator=(ForwardMaterialInstance&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the IForwardMaterialManager.
         * \return IForwardMaterialManager.
         */
        [[nodiscard]] IForwardMaterialManager& getMaterialManager() noexcept;

        /**
         * \brief Get the IForwardMaterialManager.
         * \return IForwardMaterialManager.
         */
        [[nodiscard]] const IForwardMaterialManager& getMaterialManager() const noexcept;

        [[nodiscard]] Material& getMaterial() noexcept override;

        [[nodiscard]] const Material& getMaterial() const noexcept override;

        [[nodiscard]] ForwardMaterial& getForwardMaterial() noexcept;

        [[nodiscard]] const ForwardMaterial& getForwardMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IForwardMaterialManager& manager);

        void setMaterial(ForwardMaterial& fwdMaterial);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IForwardMaterialManager* materialManager = nullptr;

        ForwardMaterial* material = nullptr;
    };
}  // namespace sol