#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-texture/fwd.h"

std::vector<sol::Texture2D*> createGuiRenderTextures(sol::TextureManager&    textureManager,
                                                     VkFormat                colorFormat,
                                                     std::array<uint32_t, 2> size,
                                                     size_t                  count);

std::vector<sol::VulkanFramebufferPtr> createGuiFramebuffers(sol::VulkanRenderPass&              renderPass,
                                                             const std::vector<sol::Texture2D*>& textures);

std::vector<sol::Texture2D*> createViewerRenderTextures(sol::TextureManager&    textureManager,
                                                        VkFormat                colorFormat,
                                                        std::array<uint32_t, 2> size,
                                                        size_t                  count);

std::vector<sol::VulkanFramebufferPtr> createViewerFramebuffers(sol::VulkanRenderPass&              renderPass,
                                                                const std::vector<sol::Texture2D*>& textures);

std::vector<sol::VulkanFramebufferPtr> createDisplayFramebuffers(sol::VulkanRenderPass&      renderPass,
                                                                 const sol::VulkanSwapchain& swapchain);
