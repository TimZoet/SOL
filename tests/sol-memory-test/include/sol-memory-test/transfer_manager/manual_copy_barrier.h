#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

////////////////////////////////////////////////////////////////
// Test includes.
////////////////////////////////////////////////////////////////

#include "testutils/utils.h"

class ManualCopyBarrier final : public bt::UnitTest<ManualCopyBarrier, bt::CompareMixin, bt::ExceptionMixin>,
                                MemoryManagerFixture
{
public:
    void operator()() override;
};