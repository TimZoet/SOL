#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <queue>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class ITraverser2
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class TraversalAction
        {
            /**
             * \brief Continue traversal as normal.
             */
            Visit = 0,

            /**
             * \brief Ignore this node and all its children.
             */
            Terminate = 1,

            /**
             * \brief Consider this node, but do not visit its children.
             */
            IgnoreChildren = 2,

            /**
             * \brief Skip this node, but still visit children.
             */
            Skip = 3
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ITraverser2() = default;

        ITraverser2(const ITraverser2&) = delete;

        ITraverser2(ITraverser2&&) = delete;

        virtual ~ITraverser2() noexcept = default;

        ITraverser2& operator=(const ITraverser2&) = delete;

        ITraverser2& operator=(ITraverser2&&) = delete;

        virtual void traverse(const Node& node) = 0;
    };

    template<Node::Type... Ts>
        requires(sizeof...(Ts) > 0)
    class Traverser : public ITraverser2
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct StackItem
        {
            const Node* previous;
            const Node* node;
        };

    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Traverser() = default;

        Traverser(const Traverser&) = delete;

        Traverser(Traverser&&) = delete;

        ~Traverser() noexcept override = default;

        Traverser& operator=(const Traverser&) = delete;

        Traverser& operator=(Traverser&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Traversal.
        ////////////////////////////////////////////////////////////////

        void traverse(const Node& node) final
        {
            traverseBegin();
            traverseImpl(node);
            traverseEnd();
        }

        virtual void traverseBegin() {}

        virtual void traverseEnd() {}

        virtual void visit(const Node& node, const Node* previous) = 0;

        virtual TraversalAction generalMask(uint64_t mask) = 0;

        virtual TraversalAction typeMask(uint64_t mask) = 0;

    private:
        void traverseImpl(const Node& root)
        {
            std::vector<StackItem> stack;
            stack.emplace_back(nullptr, &root);

            while (!stack.empty())
            {
                const auto [previous, node] = stack.back();
                stack.pop_back();

                bool visitNode     = false;
                bool visitChildren = false;

                // Use general mask to see how traversal should continue.
                switch (generalMask(node->getGeneralMask()))
                {
                case TraversalAction::Visit:
                    visitNode     = true;
                    visitChildren = true;
                    break;
                case TraversalAction::Terminate: break;
                case TraversalAction::IgnoreChildren: visitNode = true; break;
                case TraversalAction::Skip: visitChildren = true; break;
                }

                if (!visitNode && !visitChildren) continue;

                const auto supported = supportsNode(*node);

                // Use type mask to potentially override traversal actions.
                if (supported)
                {
                    switch (typeMask(node->getTypeMask()))
                    {
                    case TraversalAction::Visit:
                        visitNode &= true;
                        visitChildren &= true;
                        break;
                    case TraversalAction::Terminate:
                        visitNode     = false;
                        visitChildren = false;
                        break;
                    case TraversalAction::IgnoreChildren:
                        visitNode &= true;
                        visitChildren = false;
                        break;
                    case TraversalAction::Skip:
                        visitNode = false;
                        visitChildren &= true;
                        break;
                    }
                }

                if (!visitNode && !visitChildren) continue;

                if (supported && visitNode) visit(*node, previous);

                // Push children on stack.
                if (visitChildren)
                    for (const auto& child : *node | std::views::reverse)
                        stack.emplace_back(visitNode ? node : previous, &child);
            }
        }

        [[nodiscard]] bool supportsNode(const Node& node) const noexcept
        {
            return supportsNodeImpl<Node::Type::Empty, Ts...>(node);
        }

        template<Node::Type... T2>
        [[nodiscard]] static bool supportsNodeImpl(const Node& node) noexcept
        {
            return (node.supportsType(T2) || ...);
        }
    };

    template<std::derived_from<Node> N, typename CustomData = std::tuple<>>
    class TraversalStack
    {
    public:
        struct Empty
        {
        };

        struct Item
        {
            const size_t   parent = 0;
            const size_t   index  = 0;
            const N* const node   = nullptr;
            CustomData     data;
        };

        //template<std::same_as<CustomData> C = CustomData>
        void push(const N& node, CustomData d = {})
        {
            if (active == ~0ULL)
            {
                active = items.size();
                items.emplace_back(~0ULL, items.size(), &node, std::move(d));
                return;
            }

            while (active != ~0ULL && !node.isDescendantOf(*items[active].node)) { active = items[active].parent; }

            active = items.emplace_back(active, items.size(), &node, std::move(d)).index;
        }

        Item* getActive(const Node& node) noexcept
        {
            while (active != ~0ULL && !node.isDescendantOf(*items[active].node)) { active = items[active].parent; }
            if (active == ~0ULL) return nullptr;
            return &items[active];
        }

        Item* operator[](const size_t index) noexcept
        {
            if (index >= items.size()) return nullptr;
            return &items[index];
        }

    private:
        size_t            active = ~0ULL;
        std::vector<Item> items;
    };
#if 0
    template<std::derived_from<Node> N, typename CustomData = void>
    class TraversalStack
    {
    public:
        struct Item
        {
            const size_t   parent = 0;
            const size_t   index  = 0;
            const N* const node   = nullptr;
            CustomData     data;
        };

        void push(const N& node, CustomData d)
        {
            if (active == ~0ULL)
            {
                active = items.size();
                items.emplace_back(~0ULL, items.size(), &node);
                return;
            }

            while (active != ~0ULL && !node.isDescendantOf(*items[active].node)) { active = items[active].parent; }

            active = items.emplace_back(active, items.size(), &node, std::move(d)).index;
        }

        Item* getActive(const Node& node) noexcept
        {
            while (active != ~0ULL && !node.isDescendantOf(*items[active].node)) { active = items[active].parent; }
            if (active == ~0ULL) return nullptr;
            return &items[active];
        }

        Item* operator[](const size_t index) noexcept
        {
            if (index >= items.size()) return nullptr;
            return &items[index];
        }

    private:
        size_t            active = ~0ULL;
        std::vector<Item> items;
    };

    template<std::derived_from<Node> N>
    class TraversalStack<N, void>
    {
    public:
        struct Item
        {
            const size_t   parent = 0;
            const size_t   index  = 0;
            const N* const node   = nullptr;
        };

        void push(const N& node)
        {
            if (active == ~0ULL)
            {
                active = items.size();
                items.emplace_back(~0ULL, items.size(), &node);
                return;
            }

            while (active != ~0ULL && !node.isDescendantOf(*items[active].node)) { active = items[active].parent; }

            active = items.emplace_back(active, items.size(), &node).index;
        }

        Item* getActive(const Node& node) noexcept
        {
            while (active != ~0ULL && !node.isDescendantOf(*items[active].node)) { active = items[active].parent; }
            if (active == ~0ULL) return nullptr;
            return &items[active];
        }

        Item* operator[](const size_t index) noexcept
        {
            if (index >= items.size()) return nullptr;
            return &items[index];
        }

    private:
        size_t            active = ~0ULL;
        std::vector<Item> items;
    };
#endif
}  // namespace sol