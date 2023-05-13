#include "sol-scenegraph/node.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/scenegraph.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Node::Node() = default;

    Node::~Node() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type Node::getType() const noexcept { return Type::Empty; }

    Scenegraph& Node::getScenegraph() noexcept { return *scenegraph; }

    const Scenegraph& Node::getScenegraph() const noexcept { return *scenegraph; }

    const std::vector<NodePtr>& Node::getChildren() const noexcept { return children; }

    uint64_t Node::getGeneralMask() const noexcept { return generalMask; }

    uint64_t Node::getTypeMask() const noexcept { return typeMask; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Node::setGeneralMask(const uint64_t value) noexcept { generalMask = value; }

    void Node::setTypeMask(const uint64_t value) noexcept { typeMask = value; }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void Node::clearChildren() { children.clear(); }

    ////////////////////////////////////////////////////////////////
    // Debugging and visualization.
    ////////////////////////////////////////////////////////////////

    //void Node::visualize(dot::Graph& graph, dot::Node* parentDotNode) const
    //{
    //    // Create node.
    //    auto& node = graph.createNode();
    //    node.setLabel(getVizLabel());
    //    node.setShape(getVizShape());

    //    // Create edge between parent and this node.
    //    if (parentDotNode) graph.createEdge(*parentDotNode, node);

    //    // Visualize children.
    //    for (const auto& child : children) child->visualize(graph, &node);
    //}

    std::string Node::getVizLabel() const { return ""; }

    std::string Node::getVizShape() const { return "square"; }

    std::string Node::getVizFillColor() const { return "white"; }

    std::string Node::getVizOutlineColor() const { return "black"; }

}  // namespace sol