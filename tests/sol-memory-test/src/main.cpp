#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory-test/pool/free_at_once_memory_pool.h"
#include "sol-memory-test/pool/i_memory_pool.h"
#include "sol-memory-test/pool/non_linear_memory_pool.h"
#include "sol-memory-test/pool/ring_buffer_memory_pool.h"
#include "sol-memory-test/pool/stack_memory_pool.h"
#include "sol-memory-test/transfer_manager/concurrent_buffer_transactions.h"
#include "sol-memory-test/transfer_manager/large_copy.h"
#include "sol-memory-test/transfer_manager/manual_copy_barrier.h"
#include "sol-memory-test/transfer_manager/multiple_copies.h"
#include "sol-memory-test/transfer_manager/partial_copy.h"

#ifdef WIN32
#include "Windows.h"
#endif

int main(int argc, char** argv)
{
    // Set path next to executable.
#ifdef WIN32
    {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const std::filesystem::path workdir(path);
        const auto                  p = workdir.parent_path();
        std::filesystem::current_path(p);
    }
#endif
    // TODO: Parallel tests are not supported. BetterTest needs an option to always disable them and perhaps even give an error when trying run in parallel.
    return bt::run<FreeAtOnceMemoryPool,
                   IMemoryPool,
                   NonLinearMemoryPool,
                   RingBufferMemoryPool,
                   StackMemoryPool,

                   ConcurrentBufferTransactions,
                   LargeCopy,
                   ManualCopyBarrier,
                   MultipleCopies,
                   PartialCopy>(argc, argv, "sol-memory");
}
