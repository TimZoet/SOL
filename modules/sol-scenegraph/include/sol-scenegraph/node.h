#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cassert>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

//#include "dot/graph.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/fwd.h"

namespace sol
{
    class Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        // TODO: Set explicit values to allow reordering down the line?
        enum class Type
        {
            Empty,
            Mesh,
            GraphicsMaterial,
            GraphicsPushConstant,
            DeferredMaterial,
            ComputeMaterial,
            Dispatch,
            RayTracingMaterial,
            TraceRays,
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Node();

        Node(const Node&) = delete;

        Node(Node&&) = delete;

        virtual ~Node() noexcept;

        Node& operator=(const Node&) = delete;

        Node& operator=(Node&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual Type getType() const noexcept;

        [[nodiscard]] Scenegraph& getScenegraph() noexcept;

        [[nodiscard]] const Scenegraph& getScenegraph() const noexcept;

        [[nodiscard]] const std::vector<NodePtr>& getChildren() const noexcept;

        [[nodiscard]] uint64_t getGeneralMask() const noexcept;

        [[nodiscard]] uint64_t getTypeMask() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setGeneralMask(uint64_t value) noexcept;

        void setTypeMask(uint64_t value) noexcept;

        ////////////////////////////////////////////////////////////////
        // Child nodes.
        ////////////////////////////////////////////////////////////////

        template<std::derived_from<Node> T>
        T& addChild(std::unique_ptr<T> child)
        {
            assert(child->parent == nullptr && child->scenegraph == nullptr);

            auto& ref         = *child;
            child->parent     = this;
            child->scenegraph = scenegraph;
            children.emplace_back(std::move(child));

            return ref;
        }

        void clearChildren();

        ////////////////////////////////////////////////////////////////
        // Debugging and visualization.
        ////////////////////////////////////////////////////////////////

        //void visualize(dot::Graph& graph, dot::Node* parentDotNode) const;

        [[nodiscard]] virtual std::string getVizLabel() const;

        [[nodiscard]] virtual std::string getVizShape() const;

        [[nodiscard]] virtual std::string getVizFillColor() const;

        [[nodiscard]] virtual std::string getVizOutlineColor() const;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        Scenegraph* scenegraph = nullptr;

        Node* parent = nullptr;

        std::vector<NodePtr> children;

        uint64_t generalMask = 0;

        uint64_t typeMask = 0;
    };
}  // namespace sol