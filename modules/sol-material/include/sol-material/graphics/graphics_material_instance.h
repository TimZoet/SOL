#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>
#include <vector>

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
#include "sol-material/material.h"
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

        /**
         * \brief Get the dynamic cull mode state.
         * \return State.
         */
        [[nodiscard]] std::optional<CullMode> getCullMode() const noexcept;

        /**
         * \brief Get the dynamic front face state.
         * \return State.
         */
        [[nodiscard]] std::optional<FrontFace> getFrontFace() const noexcept;

        /**
         * \brief Get the dynamic viewport state.
         * \return State.
         */
        [[nodiscard]] const std::optional<std::vector<VkViewport>>& getViewports() const noexcept;

        /**
         * \brief Get the dynamic scissor state.
         * \return State.
         */
        [[nodiscard]] const std::optional<std::vector<VkRect2D>>& getScissors() const noexcept;

        /**
         * \brief Get the dynamic polygon mode state.
         * \return State.
         */
        [[nodiscard]] std::optional<PolygonMode> getPolygonMode() const noexcept;



        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IGraphicsMaterialManager& manager);

        void setMaterial(GraphicsMaterial& fwdMaterial);

        /**
         * \brief Set the dynamic cull mode state. Does not have any effect if the dynamic state was not enabled in the material layout.
         * \param value New state.
         */
        void setCullMode(std::optional<CullMode> value) noexcept;

        /**
         * \brief Set the dynamic front face state. Does not have any effect if the dynamic state was not enabled in the material layout.
         * \param value New state.
         */
        void setFrontFace(std::optional<FrontFace> value) noexcept;

        /**
         * \brief Set the dynamic viewport state. Does not have any effect if the dynamic state was not enabled in the material layout.
         * \param value New state.
         */
        void setViewports(std::optional<std::vector<VkViewport>> value);

        /**
         * \brief Set the dynamic scissor state. Does not have any effect if the dynamic state was not enabled in the material layout.
         * \param value New state.
         */
        void setScissors(std::optional<std::vector<VkRect2D>> value);

        /**
         * \brief Set the dynamic polygon mode state. Does not have any effect if the dynamic state was not enabled in the material layout.
         * \param value New state.
         */
        void setPolygonMode(std::optional<PolygonMode> value) noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IGraphicsMaterialManager* materialManager = nullptr;

        GraphicsMaterial* material = nullptr;

        std::optional<std::vector<VkViewport>> viewports;

        std::optional<std::vector<VkRect2D>> scissors;

        std::optional<CullMode> cullMode;

        std::optional<FrontFace> frontFace;

        std::optional<PolygonMode> polygonMode;
    };
}  // namespace sol