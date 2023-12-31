#include "sol-scenegraph/node.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <iterator>
#include <cassert>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid_system_generator.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Node::Node() : uuid(uuids::uuid_system_generator{}()) {}

    Node::Node(const uuids::uuid id) : uuid(id) {}

    Node::~Node() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const uuids::uuid& Node::getUuid() const noexcept { return uuid; }

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
    // Children.
    ////////////////////////////////////////////////////////////////

    VectorUniquePtrIterator<Node> Node::begin() { return VectorUniquePtrIterator(children.data()); }

    VectorUniquePtrIterator<Node> Node::end() { return VectorUniquePtrIterator(children.data() + children.size()); }

    Node& Node::operator[](const size_t index) { return *children.at(index); }

    const Node& Node::operator[](const size_t index) const { return *children.at(index); }

    void Node::clearChildren() { children.clear(); }

    void Node::addChildImpl(NodePtr child)
    {
        assert(child->parent == nullptr && child->scenegraph == nullptr);
        child->parent     = this;
        child->scenegraph = scenegraph;

        children.emplace_back(std::move(child));
    }

    void Node::insertChildImpl(NodePtr child, const size_t index)
    {
        child->parent     = this;
        child->scenegraph = scenegraph;

        if (index >= children.size())
            children.push_back(std::move(child));
        else
            children.insert(children.begin() + index, std::move(child));
    }

    void Node::remove(const ChildAction action)
    {
        if (!parent) throw SolError("Cannot remove node without a parent.");

        const auto it     = std::ranges::find_if(parent->children, [this](const auto& n) { return n.get() == this; });
        auto       offset = it - parent->children.begin();

        switch (action)
        {
        case ChildAction::Remove: break;
        case ChildAction::Extract: throw SolError("Cannot remove node with ChildAction::Extract.");
        case ChildAction::Append:
            for (const auto& c : children) c->parent = parent;
            parent->children.insert(parent->children.end(),
                                    std::make_move_iterator(children.begin()),
                                    std::make_move_iterator(children.end()));
            break;
        case ChildAction::Insert:
            for (const auto& c : children) c->parent = parent;
            offset += children.size();
            parent->children.insert(
              it, std::make_move_iterator(children.begin()), std::make_move_iterator(children.end()));
            break;
        case ChildAction::Prepend:
            for (const auto& c : children) c->parent = parent;
            offset += children.size();
            parent->children.insert(parent->children.begin(),
                                    std::make_move_iterator(children.begin()),
                                    std::make_move_iterator(children.end()));
            break;
        }

        // Remove self.
        parent->children.erase(parent->children.begin() + offset);
    }

}  // namespace sol
