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
    class GraphicsPushConstantNode : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsPushConstantNode() = default;

        GraphicsPushConstantNode(GraphicsMaterial& mtl);

        GraphicsPushConstantNode(const GraphicsPushConstantNode&) = delete;

        GraphicsPushConstantNode(GraphicsPushConstantNode&&) = delete;

        ~GraphicsPushConstantNode() noexcept override = default;

        GraphicsPushConstantNode& operator=(const GraphicsPushConstantNode&) = delete;

        GraphicsPushConstantNode& operator=(GraphicsPushConstantNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        /**
         * \brief Get material.
         * \return GraphicsMaterial.
         */
        [[nodiscard]] const GraphicsMaterial& getMaterial() const noexcept;

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

        GraphicsMaterial* material = nullptr;
    };
}  // namespace sol
