#include "sol-material/graphics/graphics_material_layout.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ForwardMaterialLayout::ForwardMaterialLayout(VulkanDevice& vkDevice) : MaterialLayout(vkDevice) {}

    ForwardMaterialLayout::~ForwardMaterialLayout() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////
    
    const std::vector<VkPipelineColorBlendAttachmentState>& ForwardMaterialLayout::getColorBlending() const noexcept
    {
        return colorBlending;
    }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    void ForwardMaterialLayout::addColorBlending(VkPipelineColorBlendAttachmentState state)
    {
        colorBlending.emplace_back(state);
    }
}  // namespace sol
