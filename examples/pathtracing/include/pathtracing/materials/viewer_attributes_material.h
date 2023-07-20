#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/forward/forward_material.h"
#include "sol-material/forward/forward_material_instance.h"
#include "sol-mesh/fwd.h"
#include "sol-texture/texture2d.h"

class ViewerAttributesMaterial : public sol::ForwardMaterial
{
public:
    ViewerAttributesMaterial() = default;

    ViewerAttributesMaterial(sol::VulkanShaderModuleSharedPtr vertexModule,
                             sol::VulkanShaderModuleSharedPtr fragmentModule);

    ViewerAttributesMaterial(const ViewerAttributesMaterial&) = delete;

    ViewerAttributesMaterial(ViewerAttributesMaterial&&) = delete;

    ~ViewerAttributesMaterial() noexcept override;

    ViewerAttributesMaterial& operator=(const ViewerAttributesMaterial&) = delete;

    ViewerAttributesMaterial& operator=(ViewerAttributesMaterial&&) = delete;
};

class ViewerAttributesMaterialInstance : public sol::ForwardMaterialInstance
{
public:
    ViewerAttributesMaterialInstance() = default;

    ViewerAttributesMaterialInstance(const ViewerAttributesMaterialInstance&) = delete;

    ViewerAttributesMaterialInstance(ViewerAttributesMaterialInstance&&) = delete;

    ~ViewerAttributesMaterialInstance() noexcept override;

    ViewerAttributesMaterialInstance& operator=(const ViewerAttributesMaterialInstance&) = delete;

    ViewerAttributesMaterialInstance& operator=(ViewerAttributesMaterialInstance&&) = delete;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] uint32_t getSetIndex() const override;

    [[nodiscard]] bool isUniformBufferStale(size_t binding) const override;

    [[nodiscard]] const void* getUniformBufferData(size_t binding) const override;

    [[nodiscard]] sol::Texture2D* getTextureData(size_t binding) const override;

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void setActiveAttribute(int32_t index);

private:
    int32_t activeAttribute = 0;
};
