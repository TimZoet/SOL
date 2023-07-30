#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class DoubleStackMemoryPool;
    class FreeAtOnceMemoryPool;
    class IMemoryPool;
    class MemoryManager;
    class MemoryPoolBuffer;
    class NonLinearMemoryPool;
    class RingBufferMemoryPool;
    class StackMemoryPool;

    using DoubleStackMemoryPoolPtr       = std::unique_ptr<DoubleStackMemoryPool>;
    using DoubleStackMemoryPoolSharedPtr = std::shared_ptr<DoubleStackMemoryPool>;
    using FreeAtOnceMemoryPoolPtr        = std::unique_ptr<FreeAtOnceMemoryPool>;
    using FreeAtOnceMemoryPoolSharedPtr  = std::shared_ptr<FreeAtOnceMemoryPool>;
    using IMemoryPoolPtr                 = std::unique_ptr<IMemoryPool>;
    using IMemoryPoolSharedPtr           = std::shared_ptr<IMemoryPool>;
    using MemoryManagerPtr               = std::unique_ptr<MemoryManager>;
    using MemoryManagerSharedPtr         = std::shared_ptr<MemoryManager>;
    using MemoryPoolBufferPtr            = std::unique_ptr<MemoryPoolBuffer>;
    using MemoryPoolBufferSharedPtr      = std::shared_ptr<MemoryPoolBuffer>;
    using NonLinearMemoryPoolPtr         = std::unique_ptr<NonLinearMemoryPool>;
    using NonLinearMemoryPoolSharedPtr   = std::shared_ptr<NonLinearMemoryPool>;
    using RingBufferMemoryPoolPtr        = std::unique_ptr<RingBufferMemoryPool>;
    using RingBufferMemoryPoolSharedPtr  = std::shared_ptr<RingBufferMemoryPool>;
    using StackMemoryPoolPtr             = std::unique_ptr<StackMemoryPool>;
    using StackMemoryPoolSharedPtr       = std::shared_ptr<StackMemoryPool>;
}  // namespace sol
