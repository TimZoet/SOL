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
    class GraphicsMaterialInstance : public MaterialInstance
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterialInstance();

        GraphicsMaterialInstance(const GraphicsMaterialInstance&) = delete;

        GraphicsMaterialInstance(GraphicsMaterialInstance&&) = delete;

        ~GraphicsMaterialInstance() override;

        GraphicsMaterialInstance& operator=(const GraphicsMaterialInstance&) = delete;

        GraphicsMaterialInstance& operator=(GraphicsMaterialInstance&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the IGraphicsMaterialManager.
         * \return IGraphicsMaterialManager.
         */
        [[nodiscard]] IGraphicsMaterialManager& getMaterialManager() noexcept;

        /**
         * \brief Get the IGraphicsMaterialManager.
         * \return IGraphicsMaterialManager.
         */
        [[nodiscard]] const IGraphicsMaterialManager& getMaterialManager() const noexcept;

        [[nodiscard]] Material& getMaterial() noexcept override;

        [[nodiscard]] const Material& getMaterial() const noexcept override;

        [[nodiscard]] GraphicsMaterial& getGraphicsMaterial() noexcept;

        [[nodiscard]] const GraphicsMaterial& getGraphicsMaterial() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IGraphicsMaterialManager& manager);

        void setMaterial(GraphicsMaterial& fwdMaterial);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IGraphicsMaterialManager* materialManager = nullptr;

        GraphicsMaterial* material = nullptr;
    };
}  // namespace sol