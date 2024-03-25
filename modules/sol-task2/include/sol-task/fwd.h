#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class AcquireTask;
    class CommandBufferProvider;
    class CommandBufferResource;
    class CompiledGraph;
    class CustomTask;
    class IndexProvider;
    class IndexResource;
    class IProvider;
    class ITask;
    class ITaskResource;
    class PresentTask;
    class RenderTask;
    class SubmitTask;
    class TaskGraph;

    using AcquireTaskPtr           = std::unique_ptr<AcquireTask>;
    using CommandBufferProviderPtr = std::unique_ptr<CommandBufferProvider>;
    using CommandBufferResourcePtr = std::unique_ptr<CommandBufferResource>;
    using CompiledGraphPtr         = std::unique_ptr<CompiledGraph>;
    using CustomTaskPtr            = std::unique_ptr<CustomTask>;
    using IndexProviderPtr         = std::unique_ptr<IndexProvider>;
    using IndexResourcePtr         = std::unique_ptr<IndexResource>;
    using IProviderPtr             = std::unique_ptr<IProvider>;
    using ITaskPtr                 = std::unique_ptr<ITask>;
    using ITaskResourcePtr         = std::unique_ptr<ITaskResource>;
    using PresentTaskPtr           = std::unique_ptr<PresentTask>;
    using RenderTaskPtr            = std::unique_ptr<RenderTask>;
    using SubmitTaskPtr            = std::unique_ptr<SubmitTask>;
    using TaskGraphPtr             = std::unique_ptr<TaskGraph>;
}  // namespace sol
