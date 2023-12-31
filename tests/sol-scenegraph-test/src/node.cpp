#include "sol-scenegraph-test/node.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"
#include "sol-scenegraph/scenegraph.h"

void Node::operator()()
{
    const auto scenegraph = std::make_unique<sol::Scenegraph>();

    // Check initial state.
    auto& root = scenegraph->getRootNode();
    compareNE(uuids::uuid{}, root.getUuid());
    compareEQ(sol::Node::Type::Empty, root.getType());
    compareEQ(scenegraph.get(), &root.getScenegraph());
    compareEQ(0, root.getChildren().size());
    compareEQ(0, root.getGeneralMask());
    compareEQ(0, root.getTypeMask());
    {
        size_t count = 0;
        for (const auto& child : root)
        {
            count++;
            static_cast<void>(child);
        }
        compareEQ(0, count);
    }
    expectThrow([&] { static_cast<void>(root[0]); });

    // Test masks.
    root.setGeneralMask(11);
    compareEQ(11, root.getGeneralMask());
    compareEQ(0, root.getTypeMask());
    root.setTypeMask(22);
    compareEQ(11, root.getGeneralMask());
    compareEQ(22, root.getTypeMask());

    // Add some children.
    expectNoThrow([&] {
        root.addChild(std::make_unique<sol::Node>());
        root.addChild(std::make_unique<sol::Node>());
        root.addChild(std::make_unique<sol::Node>());
    });
    compareEQ(3, root.getChildren().size());
    {
        size_t count = 0;
        for (const auto& child : root)
        {
            count++;
            static_cast<void>(child);
        }
        compareEQ(3, count);
    }

    // Insert some children.
    {
        auto  child0 = std::make_unique<sol::Node>();
        auto  child1 = std::make_unique<sol::Node>();
        auto* c0     = child0.get();
        auto* c1     = child1.get();
        expectNoThrow([&] { root.insertChild(std::move(child0), 1); });
        expectNoThrow([&] { root.insertChild(std::move(child1), 33); });
        compareEQ(&root[1], c0);
        compareEQ(&root[4], c1);
    }

    // Do a bunch of removals with different actions.
    {
        // Cannot remove root.
        expectThrow([&] { root.remove(sol::Node::ChildAction::Remove); });

        {
            // Add a child to root[0].
            auto& c = root[0].addChild(std::make_unique<sol::Node>());
            // Extract is not allowed.
            expectThrow([&] { root[0].remove(sol::Node::ChildAction::Extract); });
            // Remove root[0] but append its child to the end.
            expectNoThrow([&] { root[0].remove(sol::Node::ChildAction::Append); });
            compareEQ(5, root.getChildren().size());
            compareEQ(&c, &root[4]);
        }

        {
            // Add a child to root[1].
            auto& c = root[1].addChild(std::make_unique<sol::Node>());
            // Remove root[1] but prepend its child to the beginning.
            expectNoThrow([&] { root[1].remove(sol::Node::ChildAction::Prepend); });
            compareEQ(5, root.getChildren().size());
            compareEQ(&c, &root[0]);
        }

        {
            // Add a child to root[2].
            auto& c = root[2].addChild(std::make_unique<sol::Node>());
            // Remove root[2] but insert its child at the same spot.
            expectNoThrow([&] { root[2].remove(sol::Node::ChildAction::Insert); });
            compareEQ(5, root.getChildren().size());
            compareEQ(&c, &root[2]);
        }

        {
            // Add a child to root[3].
            root[3].addChild(std::make_unique<sol::Node>());
            // Remove root[3] including children.
            expectNoThrow([&] { root[3].remove(sol::Node::ChildAction::Remove); });
            compareEQ(4, root.getChildren().size());
        }
    }

    // Completely clear children.
    expectNoThrow([&] { root.clearChildren(); });
    compareEQ(0, root.getChildren().size());
}
