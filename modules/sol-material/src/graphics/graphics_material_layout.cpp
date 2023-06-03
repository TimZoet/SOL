#include "sol-material/graphics/graphics_material_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    GraphicsMaterialLayout::GraphicsMaterialLayout(VulkanDevice& vkDevice) : MaterialLayout(vkDevice) {}

    GraphicsMaterialLayout::~GraphicsMaterialLayout() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::vector<VkPipelineColorBlendAttachmentState>& GraphicsMaterialLayout::getColorBlending() const noexcept
    {
        return colorBlending;
    }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterialLayout::addColorBlending(VkPipelineColorBlendAttachmentState state)
    {
        colorBlending.emplace_back(state);
    }
}  // namespace sol
