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

    uint32_t GraphicsMaterialLayout::getDynamicViewportCount() const noexcept { return dynamicViewportCount; }

    uint32_t GraphicsMaterialLayout::getDynamicScissorCount() const noexcept { return dynamicScissorCount; }

    CullMode GraphicsMaterialLayout::getCullMode() const noexcept { return cullMode; }

    FrontFace GraphicsMaterialLayout::getFrontFace() const noexcept { return frontFace; }

    PolygonMode GraphicsMaterialLayout::getPolygonMode() const noexcept { return polygonMode; }

    const std::vector<VkPipelineColorBlendAttachmentState>&
      GraphicsMaterialLayout::getColorBlendAttachments() const noexcept
    {
        return colorBlending;
    }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    void GraphicsMaterialLayout::setDynamicViewportCount(const uint32_t value) { dynamicViewportCount = value; }

    void GraphicsMaterialLayout::setDynamicScissorCount(const uint32_t value) { dynamicScissorCount = value; }

    void GraphicsMaterialLayout::setCullMode(const CullMode value) noexcept { cullMode = value; }

    void GraphicsMaterialLayout::setFrontFace(const FrontFace value) noexcept { frontFace = value; }

    void GraphicsMaterialLayout::setPolygonMode(const PolygonMode value) noexcept { polygonMode = value; }

    void GraphicsMaterialLayout::addColorBlendAttachment(VkPipelineColorBlendAttachmentState state)
    {
        requireNonFinalized();
        colorBlending.emplace_back(state);
    }
}  // namespace sol
