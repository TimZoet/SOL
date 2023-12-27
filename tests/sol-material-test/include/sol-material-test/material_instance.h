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

class MaterialInstance final : public bt::UnitTest<MaterialInstance, bt::CompareMixin, bt::ExceptionMixin>, BasicFixture
{
public:
    void operator()() override;
};