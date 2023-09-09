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

class Image2DBarriers final : public bt::UnitTest<Image2DBarriers, bt::CompareMixin, bt::ExceptionMixin>,
                              MemoryManagerFixture,
                              ImageDataGeneration
{
public:
    void operator()() override;
};
