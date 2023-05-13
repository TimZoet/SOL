#include "sol-render/common/i_traverser.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ITraverser::ITraverser()
    {
        clearGeneralMaskFunction();
        clearTypeMaskFunction();
    }

    ITraverser::ITraverser(const ITraverser&) = default;

    ITraverser::ITraverser(ITraverser&&) noexcept = default;

    ITraverser::~ITraverser() noexcept = default;

    ITraverser& ITraverser::operator=(const ITraverser&) = default;

    ITraverser& ITraverser::operator=(ITraverser&&) noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const ITraverser::TraversalMaskFunction& ITraverser::getGeneralMaskFunction() const noexcept
    {
        return generalMaskFunction;
    }

    const ITraverser::TraversalMaskFunction& ITraverser::getTypeMaskFunction() const noexcept
    {
        return typeMaskFunction;
    }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void ITraverser::setGeneralMaskFunction(TraversalMaskFunction f) { generalMaskFunction = std::move(f); }

    void ITraverser::clearGeneralMaskFunction()
    {
        generalMaskFunction = [](uint64_t) { return TraversalAction::Continue; };
    }

    void ITraverser::setTypeMaskFunction(TraversalMaskFunction f) { typeMaskFunction = std::move(f); }

    void ITraverser::clearTypeMaskFunction()
    {
        typeMaskFunction = [](uint64_t) { return TraversalAction::Continue; };
    }
}  // namespace sol
