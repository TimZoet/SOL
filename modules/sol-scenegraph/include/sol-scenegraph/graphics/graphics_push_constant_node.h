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

        GraphicsPushConstantNode();

        explicit GraphicsPushConstantNode(uuids::uuid id);

        explicit GraphicsPushConstantNode(GraphicsMaterial2& mtl);

        GraphicsPushConstantNode(uuids::uuid id, GraphicsMaterial2& mtl);

        GraphicsPushConstantNode(const GraphicsPushConstantNode&) = delete;

        GraphicsPushConstantNode(GraphicsPushConstantNode&&) = delete;

        ~GraphicsPushConstantNode() noexcept override;

        GraphicsPushConstantNode& operator=(const GraphicsPushConstantNode&) = delete;

        GraphicsPushConstantNode& operator=(GraphicsPushConstantNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get material.
         * \return GraphicsMaterial.
         */
        [[nodiscard]] const GraphicsMaterial2* getMaterial() const noexcept;

        /**
         * \brief Get push constant data range.
         * \return {offset, size}.
         */
        [[nodiscard]] virtual std::pair<uint32_t, uint32_t> getRange() const noexcept = 0;

        /**
         * \brief Get stages to which the push constant data applies.
         * \return Stages.
         */
        [[nodiscard]] virtual VkShaderStageFlags getStageFlags() const noexcept = 0;

        /**
         * \brief Get push constant data.
         * \return Pointer to data.
         */
        [[nodiscard]] virtual const void* getData() const = 0;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterial(GraphicsMaterial2* mtl);

    private:
        ////////////////////////////////////////////////////////////////
        // Casting.
        ////////////////////////////////////////////////////////////////

        bool supportsTypeImpl(Type type) const noexcept override;

        void* getAsImpl(Type type) override;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterial2* material = nullptr;
    };
}  // namespace sol
