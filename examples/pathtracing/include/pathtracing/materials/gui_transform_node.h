#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol-scenegraph/forward/forward_push_constant_node.h"

class GuiTransformNode : public sol::ForwardPushConstantNode
{
public:
    GuiTransformNode() = default;

    GuiTransformNode(sol::ForwardMaterial& material);

    GuiTransformNode(const GuiTransformNode&) = delete;

    GuiTransformNode(GuiTransformNode&&) = delete;

    ~GuiTransformNode() noexcept override;

    GuiTransformNode& operator=(const GuiTransformNode&) = delete;

    GuiTransformNode& operator=(GuiTransformNode&&) = delete;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] const void* getData(size_t index) const override;

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void setScale(math::float2 value);

    void setTranslation(math::float2 value);

private:
    math::float4 transform;
};
