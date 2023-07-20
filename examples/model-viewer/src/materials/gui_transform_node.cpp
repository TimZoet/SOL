#include "pathtracing/materials/gui_transform_node.h"

GuiTransformNode::GuiTransformNode(sol::ForwardMaterial& material) :
    ForwardPushConstantNode(material), transform(1, 1, 0, 0)
{
    enablePushConstant(0);
}

GuiTransformNode::~GuiTransformNode() noexcept = default;

////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////

const void* GuiTransformNode::getData(const size_t index) const { return &transform; }

////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////

void GuiTransformNode::setScale(math::float2 value)
{
    transform.x = value.x;
    transform.y = value.y;
}

void GuiTransformNode::setTranslation(math::float2 value)
{
    transform.z = value.x;
    transform.w = value.y;
}
