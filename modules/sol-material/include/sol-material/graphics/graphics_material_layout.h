#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/static_assert.h"
#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/material.h"
#include "sol-material/material_layout.h"

namespace sol
{
    class GraphicsMaterialLayout final : public MaterialLayout
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterialLayout() = delete;

        explicit GraphicsMaterialLayout(VulkanDevice& vkDevice);

        GraphicsMaterialLayout(const GraphicsMaterialLayout&) = delete;

        GraphicsMaterialLayout(GraphicsMaterialLayout&&) = delete;

        ~GraphicsMaterialLayout() noexcept override;

        GraphicsMaterialLayout& operator=(const GraphicsMaterialLayout&) = delete;

        GraphicsMaterialLayout& operator=(GraphicsMaterialLayout&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Returns whether the specified dynamic state is enabled.
         * \tparam S Dynamic state.
         * \return True if enabled, false otherwise.
         */
        template<VkDynamicState S>
        [[nodiscard]] bool isDynamicStateEnabled() const noexcept
        {
            return const_cast<GraphicsMaterialLayout*>(this)->getDynamicState<S>();
        }

        /**
         * \brief Get the cull mode.
         * \return State.
         */
        [[nodiscard]] CullMode getCullMode() const noexcept;

        /**
         * \brief Get the front face.
         * \return State.
         */
        [[nodiscard]] FrontFace getFrontFace() const noexcept;

        /**
         * \brief Get the polygon mode.
         * \return State.
         */
        [[nodiscard]] PolygonMode getPolygonMode() const noexcept;

        [[nodiscard]] const std::vector<VkPipelineColorBlendAttachmentState>& getColorBlendAttachments() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Modifiers.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Enable or disable the specified dynamic state.
         * \tparam S Dynamic state.
         * \param enabled True to enable, false to disable.
         */
        template<VkDynamicState S>
        void enableDynamicState(const bool enabled) noexcept
        {
            requireNonFinalized();
            getDynamicState<S>() = enabled;
        }

        /**
         * \brief Set the cull mode.
         * \param value New state.
         */
        void setCullMode(CullMode value) noexcept;

        /**
         * \brief Set the dynamic front face state.
         * \param value New state.
         */
        void setFrontFace(FrontFace value) noexcept;

        /**
         * \brief Set the dynamic polygon mode state.
         * \param value New state.
         */
        void setPolygonMode(PolygonMode value) noexcept;

        void addColorBlendAttachment(VkPipelineColorBlendAttachmentState state);

    private:
        template<VkDynamicState S>
        bool& getDynamicState() noexcept
        {
            if constexpr (S == VK_DYNAMIC_STATE_VIEWPORT)
                return enabledDynamicStates.viewport;
            else if constexpr (S == VK_DYNAMIC_STATE_SCISSOR)
                return enabledDynamicStates.scissor;
            else if constexpr (S == VK_DYNAMIC_STATE_CULL_MODE)
                return enabledDynamicStates.cullMode;
            else if constexpr (S == VK_DYNAMIC_STATE_FRONT_FACE)
                return enabledDynamicStates.frontFace;
            else if constexpr (S == VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT)
                return enabledDynamicStates.viewportCount;
            else if constexpr (S == VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT)
                return enabledDynamicStates.scissorCount;
            else if constexpr (S == VK_DYNAMIC_STATE_POLYGON_MODE_EXT)
                return enabledDynamicStates.polygonMode;
            else
            {
                constexpr_static_assert<!std::same_as<decltype(S), VkDynamicState>>();
                return enabledDynamicStates.viewport;
            }
        }
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        struct
        {
            bool viewport      = false;
            bool scissor       = false;
            bool cullMode      = false;
            bool frontFace     = false;
            bool viewportCount = false;
            bool scissorCount  = false;
            bool polygonMode   = false;
        } enabledDynamicStates;

        CullMode cullMode = CullMode::None;

        FrontFace frontFace = FrontFace::CounterClockwise;

        PolygonMode polygonMode = PolygonMode::Fill;

        std::vector<VkPipelineColorBlendAttachmentState> colorBlending;
    };
}  // namespace sol
