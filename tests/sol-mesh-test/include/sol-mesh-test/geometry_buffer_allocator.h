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

class GeometryBufferAllocator final
    : public bt::UnitTest<GeometryBufferAllocator, bt::CompareMixin, bt::ExceptionMixin>,
      BasicFixture
{
public:
    void operator()() override;
};
