#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class ITraverser
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
            Continue = 0,

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

        using TraversalMaskFunction = std::function<TraversalAction(uint64_t)>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ITraverser();

        ITraverser(const ITraverser&);

        ITraverser(ITraverser&&) noexcept;

        virtual ~ITraverser() noexcept;

        ITraverser& operator=(const ITraverser&);

        ITraverser& operator=(ITraverser&&) noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual bool supportsNodeType(Node::Type type) const noexcept = 0;

        [[nodiscard]] const TraversalMaskFunction& getGeneralMaskFunction() const noexcept;

        [[nodiscard]] const TraversalMaskFunction& getTypeMaskFunction() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setGeneralMaskFunction(TraversalMaskFunction f);

        void clearGeneralMaskFunction();

        void setTypeMaskFunction(TraversalMaskFunction f);

        void clearTypeMaskFunction();

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        TraversalMaskFunction generalMaskFunction;

        TraversalMaskFunction typeMaskFunction;
    };

}  // namespace sol
