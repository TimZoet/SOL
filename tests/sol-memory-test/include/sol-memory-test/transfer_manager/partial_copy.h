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

class PartialCopy final : public bt::UnitTest<PartialCopy, bt::CompareMixin, bt::ExceptionMixin>, BasicFixture
{
public:
    void operator()() override;
};
