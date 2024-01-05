#include "sol-scenegraph-test/graphics/graphics_push_constant_node.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-material/graphics/graphics_material_instance2.h"
#include "sol-scenegraph/graphics/graphics_push_constant_node.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "testutils/materials.h"

namespace
{
    class TestNode : public sol::GraphicsPushConstantNode
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TestNode() = default;

        explicit TestNode(const uuids::uuid id) : GraphicsPushConstantNode(id) {}

        explicit TestNode(sol::GraphicsMaterial2& mtl) : GraphicsPushConstantNode(mtl) {}

        TestNode(const uuids::uuid id, sol::GraphicsMaterial2& mtl) : GraphicsPushConstantNode(id, mtl) {}

        TestNode(const TestNode&) = delete;

        TestNode(TestNode&&) = delete;

        ~TestNode() noexcept override = default;

        TestNode& operator=(const TestNode&) = delete;

        TestNode& operator=(TestNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] size_t getRangeIndex() const noexcept override { return 0; }

        [[nodiscard]] VkShaderStageFlags getStageFlags() const noexcept override
        {
            return VK_SHADER_STAGE_ALL_GRAPHICS;
        }

        [[nodiscard]] const void* getData() const override { return data.data(); }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::array<float, 8> data{};
    };
}  // namespace

void GraphicsPushConstantNode::operator()()
{
    const auto [descriptorLayouts, material] = Materials::load(Materials::Graphics::Name::Simple, getDevice());

    /*
     * Test all constructors.
     */

    {
        const std::unique_ptr<sol::GraphicsPushConstantNode> node = std::make_unique<TestNode>();
        compareNE(uuids::uuid{}, node->getUuid());
        compareEQ(nullptr, node->getMaterial());
    }

    {
        const auto                                           id   = uuids::uuid_system_generator{}();
        const std::unique_ptr<sol::GraphicsPushConstantNode> node = std::make_unique<TestNode>(id);
        compareEQ(id, node->getUuid());
        compareEQ(nullptr, node->getMaterial());
    }

    {
        const auto                                           id   = uuids::uuid_system_generator{}();
        const std::unique_ptr<sol::GraphicsPushConstantNode> node = std::make_unique<TestNode>(id, *material);
        compareEQ(id, node->getUuid());
        compareEQ(material.get(), node->getMaterial());
    }

    {
        const std::unique_ptr<sol::GraphicsPushConstantNode> node = std::make_unique<TestNode>(*material);
        compareNE(uuids::uuid{}, node->getUuid());
        compareEQ(material.get(), node->getMaterial());
    }

    {
        const std::unique_ptr<sol::GraphicsPushConstantNode> node = std::make_unique<TestNode>(*material);
        compareNE(uuids::uuid{}, node->getUuid());
        compareEQ(material.get(), node->getMaterial());
    }

    /*
     * Test remaining methods.
     */

    {
        const std::unique_ptr<sol::GraphicsPushConstantNode> node = std::make_unique<TestNode>();
        compareTrue(node->supportsType(sol::Node::Type::Empty));
        compareTrue(node->supportsType(sol::Node::Type::GraphicsPushConstant));
        compareEQ(node.get(), node->getAs(sol::Node::Type::Empty));
        compareEQ(node.get(), node->getAs(sol::Node::Type::GraphicsPushConstant));
        compareEQ(nullptr, node->getMaterial());
        expectNoThrow([&] { node->setMaterial(material.get()); });
        compareEQ(material.get(), node->getMaterial());
        compareEQ(0, node->getRangeIndex());
        compareEQ(VK_SHADER_STAGE_ALL_GRAPHICS, node->getStageFlags());
        compareNE(nullptr, node->getData());
    }
}
