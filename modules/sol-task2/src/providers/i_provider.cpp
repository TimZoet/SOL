#include "sol-task/providers/i_provider.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    IProvider::IProvider(CompiledGraph& g) : graph(&g) {}

    IProvider::~IProvider() noexcept = default;
}  // namespace sol
