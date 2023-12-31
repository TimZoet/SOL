#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"
#include "sol-material/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class GraphicsDynamicStateNode : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsDynamicStateNode();

        explicit GraphicsDynamicStateNode(uuids::uuid id);

        GraphicsDynamicStateNode(const GraphicsDynamicStateNode&) = delete;

        GraphicsDynamicStateNode(GraphicsDynamicStateNode&&) = delete;

        ~GraphicsDynamicStateNode() noexcept override;

        GraphicsDynamicStateNode& operator=(const GraphicsDynamicStateNode&) = delete;

        GraphicsDynamicStateNode& operator=(GraphicsDynamicStateNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        [[nodiscard]] std::vector<GraphicsDynamicStatePtr>& getStates() noexcept;

        [[nodiscard]] const std::vector<GraphicsDynamicStatePtr>& getStates() const noexcept;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::vector<GraphicsDynamicStatePtr> states;
    };
}  // namespace sol
