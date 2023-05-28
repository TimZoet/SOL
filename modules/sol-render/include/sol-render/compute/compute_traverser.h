#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/common/i_traverser.h"
#include "sol-render/compute/fwd.h"

namespace sol
{
    class ComputeTraverser : public ITraverser
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeTraverser();

        ComputeTraverser(const ComputeTraverser&) = delete;

        ComputeTraverser(ComputeTraverser&&) = delete;

        ~ComputeTraverser() noexcept override;

        ComputeTraverser& operator=(const ComputeTraverser&) = delete;

        ComputeTraverser& operator=(ComputeTraverser&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool supportsNodeType(Node::Type type) const noexcept override;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////
        // Traversal.
        ////////////////////////////////////////////////////////////////

        void traverse(const Scenegraph& scenegraph, ComputeRenderData& renderData);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////
    };
}  // namespace sol