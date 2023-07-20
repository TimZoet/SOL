#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol-material/forward/forward_material.h"
#include "sol-scenegraph/forward/forward_push_constant_node.h"

class ModelTransformNode : public sol::ForwardPushConstantNode
{
public:
    ModelTransformNode() = default;

    ModelTransformNode(sol::ForwardMaterial& material);

    ModelTransformNode(const ModelTransformNode&) = delete;

    ModelTransformNode(ModelTransformNode&&) = delete;

    ~ModelTransformNode() noexcept override;

    ModelTransformNode& operator=(const ModelTransformNode&) = delete;

    ModelTransformNode& operator=(ModelTransformNode&&) = delete;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] const math::mat4x4f& getModel() const noexcept;

    [[nodiscard]] const void* getData(size_t index) const override;

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void setModel(math::mat4x4f m);

private:
    math::mat4x4f model;
};
