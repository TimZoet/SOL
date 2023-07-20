#include "pathtracing/materials/model_transform_node.h"

ModelTransformNode::ModelTransformNode(sol::ForwardMaterial& material) :
    ForwardPushConstantNode(material), model(math::identity<math::mat4x4f>())
{
    enablePushConstant(0);
}

ModelTransformNode::~ModelTransformNode() noexcept = default;

////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////

const math::mat4x4f& ModelTransformNode::getModel() const noexcept { return model; }

const void* ModelTransformNode::getData(const size_t) const { return &model; }

////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////

void ModelTransformNode::setModel(math::mat4x4f m) { model = std::move(m); }
