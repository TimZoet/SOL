#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class DispatchNode final : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        DispatchNode() = default;

        explicit DispatchNode(std::array<uint32_t, 3> value);

        DispatchNode(uint32_t x, uint32_t y, uint32_t z);

        DispatchNode(const DispatchNode&) = delete;

        DispatchNode(DispatchNode&&) = delete;

        ~DispatchNode() noexcept override = default;

        DispatchNode& operator=(const DispatchNode&) = delete;

        DispatchNode& operator=(DispatchNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        [[nodiscard]] std::array<uint32_t, 3> getGroupCount() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setGroupCount(std::array<uint32_t, 3> value) noexcept;

        void setGroupCount(uint32_t x, uint32_t y, uint32_t z) noexcept;

        ////////////////////////////////////////////////////////////////
        // Debugging and visualization.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string getVizLabel() const override;

        [[nodiscard]] std::string getVizShape() const override;

        [[nodiscard]] std::string getVizFillColor() const override;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::array<uint32_t, 3> count{0, 0, 0};
    };
}  // namespace sol