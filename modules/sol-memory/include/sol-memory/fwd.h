#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class Buffer;
    class DoubleStackMemoryPool;
    class FreeAtOnceMemoryPool;
    class IBuffer;
    class IBufferAllocator;
    class IMemoryPool;
    class MemoryManager;
    class MemoryPoolBuffer;
    class NonLinearMemoryPool;
    class RingBufferMemoryPool;
    class StackMemoryPool;

    using BufferPtr                      = std::unique_ptr<Buffer>;
    using BufferSharedPtr                = std::shared_ptr<Buffer>;
    using DoubleStackMemoryPoolPtr       = std::unique_ptr<DoubleStackMemoryPool>;
    using DoubleStackMemoryPoolSharedPtr = std::shared_ptr<DoubleStackMemoryPool>;
    using FreeAtOnceMemoryPoolPtr        = std::unique_ptr<FreeAtOnceMemoryPool>;
    using FreeAtOnceMemoryPoolSharedPtr  = std::shared_ptr<FreeAtOnceMemoryPool>;
    using IBufferPtr                     = std::unique_ptr<IBuffer>;
    using IBufferSharedPtr               = std::shared_ptr<IBuffer>;
    using IBufferAllocatorPtr            = std::unique_ptr<IBufferAllocator>;
    using IBufferAllocatorSharedPtr      = std::shared_ptr<IBufferAllocator>;
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
