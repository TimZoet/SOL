#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

namespace sol
{
    VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                           VkDebugUtilsMessageTypeFlagsEXT             messageType,
                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                           void*                                       pUserData);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks*              pAllocator,
                                          VkDebugUtilsMessengerEXT*                 pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance                   instance,
                                       VkDebugUtilsMessengerEXT     debugMessenger,
                                       const VkAllocationCallbacks* pAllocator);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void setupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT& debugMessenger);
}  // namespace sol