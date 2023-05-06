#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

sol::VulkanInstancePtr createDefaultInstance();

sol::VulkanPhysicalDevicePtr createDefaultPhysicalDevice(sol::VulkanInstance& instance);

sol::VulkanDevicePtr createDefaultDevice(sol::VulkanPhysicalDevice& physicalDevice);
