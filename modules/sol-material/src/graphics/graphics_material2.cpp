#include "sol-material/graphics/graphics_material2.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsMaterial2::GraphicsMaterial2(VulkanGraphicsPipeline2Ptr           graphicsPipeline,
                                         std::vector<const DescriptorLayout*> layouts) :
        Material2(graphicsPipeline->getDevice(), std::move(layouts)), pipeline(std::move(graphicsPipeline))
    {
        for (const auto d : pipeline->getDynamicStates()) dynamicStates.insert(fromVulkanEnum(d));
    }

    GraphicsMaterial2::~GraphicsMaterial2() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const VulkanGraphicsPipeline2& GraphicsMaterial2::getPipeline() const noexcept { return *pipeline; }

    const std::set<GraphicsDynamicState::StateType>& GraphicsMaterial2::getDynamicStates() const noexcept
    {
        return dynamicStates;
    }

    bool GraphicsMaterial2::isDynamicStateEnabled(const GraphicsDynamicState::StateType state) const noexcept
    {
        return dynamicStates.contains(state);
    }

}  // namespace sol
