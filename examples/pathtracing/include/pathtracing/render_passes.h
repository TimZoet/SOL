#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_render_pass.h"

sol::VulkanRenderPassPtr createGuiRenderPass(sol::VulkanDevice& device, VkFormat colorFormat);

sol::VulkanRenderPassPtr createViewerRenderPass(sol::VulkanDevice& device, VkFormat colorFormat, VkFormat depthFormat);

sol::VulkanRenderPassPtr createDisplayRenderPass(sol::VulkanDevice& device, VkFormat colorFormat);