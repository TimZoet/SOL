#include "sol-material/graphics/graphics_material_layout.h"

#include "sol-error/sol_error.h"

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

    const MeshLayout* GraphicsMaterialLayout::getMeshLayout() const noexcept { return meshLayout; }

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

    void GraphicsMaterialLayout::finalize(MaterialLayoutDescription desc)
    {
        if (!meshLayout || !meshLayout->isFinalized()) throw SolError("Cannot finalize GraphicsMateralLayout without finalized MeshLayout.");
        MaterialLayout::finalize(std::move(desc));
    }

    void GraphicsMaterialLayout::setMeshLayout(MeshLayout& mLayout)
    {
        requireNonFinalized();
        meshLayout = &mLayout;
    }

    void GraphicsMaterialLayout::setDynamicViewportCount(const uint32_t value)
    {
        requireNonFinalized();
        dynamicViewportCount = value;
    }

    void GraphicsMaterialLayout::setDynamicScissorCount(const uint32_t value)
    {
        requireNonFinalized();
        dynamicScissorCount = value;
    }

    void GraphicsMaterialLayout::setCullMode(const CullMode value) noexcept
    {
        requireNonFinalized();
        cullMode = value;
    }

    void GraphicsMaterialLayout::setFrontFace(const FrontFace value) noexcept
    {
        requireNonFinalized();
        frontFace = value;
    }

    void GraphicsMaterialLayout::setPolygonMode(const PolygonMode value) noexcept
    {
        requireNonFinalized();
        polygonMode = value;
    }

    void GraphicsMaterialLayout::addColorBlendAttachment(VkPipelineColorBlendAttachmentState state)
    {
        requireNonFinalized();
        colorBlending.emplace_back(state);
    }
}  // namespace sol
