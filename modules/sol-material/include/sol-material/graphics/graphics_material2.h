#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <format>
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

#include "sol-material/fwd.h"
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

        ////////////////////////////////////////////////////////////////
        // Instances.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new material instance for this material.
         * \tparam T Material instance type.
         * \tparam Args Additional constructor argument types.
         * \param args Additional constructor arguments.
         * \return New material instance.
         */
        template<std::derived_from<GraphicsMaterialInstance2> T, typename... Args>
        [[nodiscard]] std::unique_ptr<T> createInstance(Args&&... args)
        {
            return std::make_unique<T>(*this, std::forward<Args>(args)...);
        }

        /**
         * \brief Create a new dynamic state object. While the state object is not explicitly linked to this material,
         * trying to create a dynamic state type that was not enabled will throw.
         * \tparam T Dynamic state type.
         * \tparam Args Additional constructor argument types.
         * \param args Additional constructor arguments.
         * \return New dynamic state object.
         */
        template<std::derived_from<GraphicsDynamicState> T, typename... Args>
        [[nodiscard]] std::unique_ptr<T> createDynamicState(Args&&... args)
        {
            if (!isDynamicStateEnabled(T::template type))
                throw SolError(
                  std::format("Cannot create dynamic state {} because it was not enabled for this material.",
                              static_cast<std::underlying_type_t<GraphicsDynamicState::StateType>>(T::template type)));

            return std::make_unique<T>(std::forward<Args>(args)...);
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanGraphicsPipeline2Ptr pipeline;

        std::set<GraphicsDynamicState::StateType> dynamicStates;
    };
}  // namespace sol
