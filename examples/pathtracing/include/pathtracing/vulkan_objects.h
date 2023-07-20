#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_device.h"
#include "sol-core/vulkan_instance.h"
#include "sol-core/vulkan_physical_device.h"
#include "sol-core/vulkan_surface.h"
#include "sol-core/vulkan_swapchain.h"
#include "sol-window/fwd.h"
#include "sol-window/window.h"

sol::WindowPtr createWindow(int32_t width, int32_t height);

sol::VulkanInstancePtr createInstance();

sol::VulkanSurfacePtr createSurface(sol::VulkanInstance& instance, sol::Window& window);

sol::VulkanPhysicalDevicePtr createPhysicalDevice(sol::VulkanInstance& instance, sol::VulkanSurface& surface);

sol::VulkanDevicePtr createDevice(sol::VulkanPhysicalDevice& physicalDevice);

sol::VulkanSwapchainPtr createSwapchain(sol::VulkanDevice& device, VkExtent2D extent);
