#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <utility>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_material.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class ForwardPushConstantNode : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ForwardPushConstantNode() = default;

        ForwardPushConstantNode(ForwardMaterial& mtl);

        ForwardPushConstantNode(const ForwardPushConstantNode&) = delete;

        ForwardPushConstantNode(ForwardPushConstantNode&&) = delete;

        ~ForwardPushConstantNode() noexcept override = default;

        ForwardPushConstantNode& operator=(const ForwardPushConstantNode&) = delete;

        ForwardPushConstantNode& operator=(ForwardPushConstantNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        /**
         * \brief Get forward material.
         * \return ForwardMaterial.
         */
        [[nodiscard]] const ForwardMaterial& getMaterial() const noexcept;

        /**
         * \brief Get push constant data range.
         * \return Range.
         */
        [[nodiscard]] virtual std::pair<uint32_t, uint32_t> getRange() const noexcept = 0;

        [[nodiscard]] virtual VkShaderStageFlags getStageFlags() const noexcept = 0;

        /**
         * \brief Get push constant data.
         * \return Pointer to data.
         */
        [[nodiscard]] virtual const void* getData() const = 0;

        ////////////////////////////////////////////////////////////////
        // Debugging and visualization.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string getVizLabel() const override;

        [[nodiscard]] std::string getVizShape() const override;

        [[nodiscard]] std::string getVizFillColor() const override;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        ForwardMaterial* material = nullptr;
    };
}  // namespace sol
