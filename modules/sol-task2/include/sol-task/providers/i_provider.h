#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-task/fwd.h"

namespace sol
{
    class IProvider
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IProvider() = delete;

        explicit IProvider(CompiledGraph& g);

        IProvider(const IProvider&) = delete;

        IProvider(IProvider&&) = delete;

        virtual ~IProvider() noexcept;

        IProvider& operator=(const IProvider&) = delete;

        IProvider& operator=(IProvider&&) = delete;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        CompiledGraph* graph = nullptr;
    };
}  // namespace sol
