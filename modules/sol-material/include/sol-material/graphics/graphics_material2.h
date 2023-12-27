#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <set>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/vulkan_graphics_pipeline2.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/material2.h"
#include "sol-material/graphics/graphics_dynamic_state.h"

namespace sol
{
    class GraphicsMaterial2 : public Material2
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterial2() = delete;

        GraphicsMaterial2(VulkanGraphicsPipeline2Ptr graphicsPipeline, std::vector<const DescriptorLayout*> layouts);

        GraphicsMaterial2(const GraphicsMaterial2&) = delete;

        GraphicsMaterial2(GraphicsMaterial2&&) = delete;

        ~GraphicsMaterial2() noexcept override;

        GraphicsMaterial2& operator=(const GraphicsMaterial2&) = delete;

        GraphicsMaterial2& operator=(GraphicsMaterial2&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] const VulkanGraphicsPipeline2& getPipeline() const noexcept;

        /**
         * \brief Get the list of dynamic states enabled for this material.
         * \return List of dynamic states.
         */
        [[nodiscard]] const std::set<GraphicsDynamicState::StateType>& getDynamicStates() const noexcept;

        /**
         * \brief Check if the given dynamic state is enabled.
         * \param state Dynamic state.
         * \return True if enabled, false otherwise.
         */
        [[nodiscard]] bool isDynamicStateEnabled(GraphicsDynamicState::StateType state) const noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanGraphicsPipeline2Ptr pipeline;

        std::set<GraphicsDynamicState::StateType> dynamicStates;
    };
}  // namespace sol
