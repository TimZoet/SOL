#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class CommandQueue;
    class ICommand;

    using CommandQueuePtr       = std::unique_ptr<CommandQueue>;
    using CommandQueueSharedPtr = std::shared_ptr<CommandQueue>;
    using ICommandPtr           = std::unique_ptr<ICommand>;
    using ICommandSharedPtr     = std::shared_ptr<ICommand>;
}  // namespace sol