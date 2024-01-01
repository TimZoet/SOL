#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <uuid.h>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/utils.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/fwd.h"

namespace sol
{
    class Node
    {
    public:
        friend class Scenegraph;

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Type : uint32_t
        {
            Empty = 0,

            GraphicsDynamicState = 100,
            GraphicsMaterial     = 101,
            GraphicsPushConstant = 102,

            ComputeMaterial = 200,
            ComputeDispatch = 201,

            RayTracingMaterial = 300,
            RayTracingDispatch = 301,

            Mesh = 400,
        };

        enum class ChildAction
        {
            /**
             * \brief Remove child nodes completely.
             */
            Remove,

            /**
             * \brief Extract child nodes together with node.
             */
            Extract,

            /**
             * \brief Add child nodes to the beginning of the list of children of the node's parent.
             */
            Prepend,

            /**
             * \brief Insert child nodes into the list of children of the node's parent at the same position.
             */
            Insert,

            /**
             * \brief Add child nodes to the end of the list of children of the node's parent.
             */
            Append
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Node();

        explicit Node(uuids::uuid id);

        Node(const Node&) = delete;

        Node(Node&&) = delete;

        virtual ~Node() noexcept;

        Node& operator=(const Node&) = delete;

        Node& operator=(Node&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the UUID.
         * \return UUID.
         */
        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

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
        // Casting.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool supportsType(Type type) const noexcept;

        [[nodiscard]] void* getAs(Type type);

    protected:
        [[nodiscard]] virtual bool supportsTypeImpl(Type type) const noexcept;

        [[nodiscard]] virtual void* getAsImpl(Type type);

    public:
        ////////////////////////////////////////////////////////////////
        // Children.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VectorUniquePtrIterator<Node> begin();

        [[nodiscard]] VectorUniquePtrIterator<Node> end();

        [[nodiscard]] Node& operator[](size_t index);

        [[nodiscard]] const Node& operator[](size_t index) const;

        /**
         * \brief Add a node to the list of children.
         * \tparam T Node type.
         * \param child Node.
         * \return Node.
         */
        template<std::derived_from<Node> T>
        T& addChild(std::unique_ptr<T> child)
        {
            auto& ref = *child;
            addChildImpl(std::move(child));
            return ref;
        }

        /**
         * \brief Insert a child node into the list of children.
         * \tparam T Node type.
         * \param child Node.
         * \param index Index into list. If index exceeds size of list, the child is added to the end of the list.
         * \return Node.
         */
        template<std::derived_from<Node> T>
        T& insertChild(std::unique_ptr<T> child, const size_t index)
        {
            auto& ref = *child;
            insertChildImpl(std::move(child), index);
            return ref;
        }

        /**
         * \brief Remove / delete node.
         * \param action What to do with any child nodes.
         */
        void remove(ChildAction action);

        //[[nodiscard]] NodePtr extract(ChildAction action);

        /**
         * \brief Recursively clear child nodes.
         */
        void clearChildren();

    protected:
        void addChildImpl(NodePtr child);

        void insertChildImpl(NodePtr child, size_t index);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        uuids::uuid uuid;

        Scenegraph* scenegraph = nullptr;

        Node* parent = nullptr;

        std::vector<NodePtr> children;

        uint64_t generalMask = 0;

        uint64_t typeMask = 0;
    };
}  // namespace sol
