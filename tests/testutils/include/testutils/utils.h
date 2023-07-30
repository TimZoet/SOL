#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

class BasicFixture
{
public:
    BasicFixture();

    virtual ~BasicFixture() noexcept;

    [[nodiscard]] static sol::VulkanInstance& getInstance();

    [[nodiscard]] static sol::VulkanPhysicalDevice& getPhysicalDevice();

    [[nodiscard]] static sol::VulkanDevice& getDevice();
};
