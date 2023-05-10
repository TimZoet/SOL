#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

namespace sol
{
    class IMeshTransfer
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IMeshTransfer();

        IMeshTransfer(const IMeshTransfer&) = delete;

        IMeshTransfer(IMeshTransfer&&) = delete;

        virtual ~IMeshTransfer() noexcept;

        IMeshTransfer& operator=(const IMeshTransfer&) = delete;

        IMeshTransfer& operator=(IMeshTransfer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Transfer.
        ////////////////////////////////////////////////////////////////

        virtual void stageCopy(MeshDescriptionPtr meshDescription, IMesh& mesh) = 0;

        virtual void transfer() = 0;
    };
}  // namespace sol
