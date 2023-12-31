#include "sol-scenegraph-test/graphics/graphics_push_constant_node.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-descriptor/descriptor_layout.h"
#include "sol-material/graphics/graphics_material2.h"
#include "sol-material/graphics/graphics_material_instance2.h"
#include "sol-scenegraph/graphics/graphics_push_constant_node.h"

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

        [[nodiscard]] std::pair<uint32_t, uint32_t> getRange() const noexcept override { return {0u, 32u}; }

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
    // Use utility function to create a simple material.
    auto [pipeline, descriptorLayouts] = createSimpleGraphicsPipeline();
    const auto layouts                 = sol::raw(descriptorLayouts);
    auto       material =
      std::make_unique<sol::GraphicsMaterial2>(uuids::uuid_system_generator{}(), std::move(pipeline), layouts);

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
        compareEQ(sol::Node::Type::GraphicsPushConstant, node->getType());
        compareEQ(nullptr, node->getMaterial());
        expectNoThrow([&] { node->setMaterial(material.get()); });
        compareEQ(material.get(), node->getMaterial());
        compareEQ(std::make_pair(0u, 32u), node->getRange());
        compareEQ(VK_SHADER_STAGE_ALL_GRAPHICS, node->getStageFlags());
        compareNE(nullptr, node->getData());
    }
}
