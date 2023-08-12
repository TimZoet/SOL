#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class Version;
    class VulkanAttachment;
    class VulkanBottomLevelAccelerationStructure;
    class VulkanBuffer;
    class VulkanCommandBuffer;
    class VulkanCommandBufferList;
    class VulkanCommandPool;
    class VulkanComputePipeline;
    class VulkanDescriptorPool;
    class VulkanDescriptorSetLayout;
    class VulkanDevice;
    class VulkanDeviceMemory;
    class VulkanFence;
    class VulkanFramebuffer;
    class VulkanGraphicsPipeline;
    class VulkanImage;
    class VulkanImageView;
    class VulkanInstance;
    class VulkanMemoryAllocator;
    class VulkanMemoryPool;
    class VulkanPhysicalDevice;
    class VulkanQueue;
    class VulkanQueueFamily;
    class VulkanRayTracingPipeline;
    class VulkanRenderPass;
    class VulkanRenderPassLayout;
    class VulkanSampler;
    class VulkanSemaphore;
    class VulkanShaderBindingTable;
    class VulkanShaderModule;
    class VulkanSubpass;
    class VulkanSurface;
    class VulkanSwapchain;
    class VulkanSwapchainSupportDetails;
    class VulkanTimelineSemaphore;
    class VulkanTopLevelAccelerationStructure;

    using VulkanAttachmentPtr                             = std::unique_ptr<VulkanAttachment>;
    using VulkanAttachmentSharedPtr                       = std::shared_ptr<VulkanAttachment>;
    using VulkanCommandBufferPtr                          = std::unique_ptr<VulkanCommandBuffer>;
    using VulkanCommandBufferSharedPtr                    = std::shared_ptr<VulkanCommandBuffer>;
    using VulkanBottomLevelAccelerationStructurePtr       = std::unique_ptr<VulkanBottomLevelAccelerationStructure>;
    using VulkanBottomLevelAccelerationStructureSharedPtr = std::shared_ptr<VulkanBottomLevelAccelerationStructure>;
    using VulkanBufferPtr                                 = std::unique_ptr<VulkanBuffer>;
    using VulkanBufferSharedPtr                           = std::shared_ptr<VulkanBuffer>;
    using VulkanCommandBufferListPtr                      = std::unique_ptr<VulkanCommandBufferList>;
    using VulkanCommandBufferListSharedPtr                = std::shared_ptr<VulkanCommandBufferList>;
    using VulkanCommandPoolPtr                            = std::unique_ptr<VulkanCommandPool>;
    using VulkanCommandPoolSharedPtr                      = std::shared_ptr<VulkanCommandPool>;
    using VulkanComputePipelinePtr                        = std::unique_ptr<VulkanComputePipeline>;
    using VulkanComputePipelineSharedPtr                  = std::shared_ptr<VulkanComputePipeline>;
    using VulkanDescriptorPoolPtr                         = std::unique_ptr<VulkanDescriptorPool>;
    using VulkanDescriptorPoolSharedPtr                   = std::shared_ptr<VulkanDescriptorPool>;
    using VulkanDescriptorSetLayoutPtr                    = std::unique_ptr<VulkanDescriptorSetLayout>;
    using VulkanDescriptorSetLayoutSharedPtr              = std::shared_ptr<VulkanDescriptorSetLayout>;
    using VulkanDevicePtr                                 = std::unique_ptr<VulkanDevice>;
    using VulkanDeviceSharedPtr                           = std::shared_ptr<VulkanDevice>;
    using VulkanDeviceMemoryPtr                           = std::unique_ptr<VulkanDeviceMemory>;
    using VulkanDeviceMemorySharedPtr                     = std::shared_ptr<VulkanDeviceMemory>;
    using VulkanFencePtr                                  = std::unique_ptr<VulkanFence>;
    using VulkanFenceSharedPtr                            = std::shared_ptr<VulkanFence>;
    using VulkanFramebufferPtr                            = std::unique_ptr<VulkanFramebuffer>;
    using VulkanFramebufferSharedPtr                      = std::shared_ptr<VulkanFramebuffer>;
    using VulkanGraphicsPipelinePtr                       = std::unique_ptr<VulkanGraphicsPipeline>;
    using VulkanGraphicsPipelineSharedPtr                 = std::shared_ptr<VulkanGraphicsPipeline>;
    using VulkanImagePtr                                  = std::unique_ptr<VulkanImage>;
    using VulkanImageSharedPtr                            = std::shared_ptr<VulkanImage>;
    using VulkanImageViewPtr                              = std::unique_ptr<VulkanImageView>;
    using VulkanImageViewSharedPtr                        = std::shared_ptr<VulkanImageView>;
    using VulkanInstancePtr                               = std::unique_ptr<VulkanInstance>;
    using VulkanInstanceSharedPtr                         = std::shared_ptr<VulkanInstance>;
    using VulkanMemoryAllocatorPtr                        = std::unique_ptr<VulkanMemoryAllocator>;
    using VulkanMemoryAllocatorSharedPtr                  = std::shared_ptr<VulkanMemoryAllocator>;
    using VulkanMemoryPoolPtr                             = std::unique_ptr<VulkanMemoryPool>;
    using VulkanMemoryPoolSharedPtr                       = std::shared_ptr<VulkanMemoryPool>;
    using VulkanPhysicalDevicePtr                         = std::unique_ptr<VulkanPhysicalDevice>;
    using VulkanPhysicalDeviceSharedPtr                   = std::shared_ptr<VulkanPhysicalDevice>;
    using VulkanQueuePtr                                  = std::unique_ptr<VulkanQueue>;
    using VulkanQueueSharedPtr                            = std::shared_ptr<VulkanQueue>;
    using VulkanRayTracingPipelinePtr                     = std::unique_ptr<VulkanRayTracingPipeline>;
    using VulkanRayTracingPipelineSharedPtr               = std::shared_ptr<VulkanRayTracingPipeline>;
    using VulkanRenderPassPtr                             = std::unique_ptr<VulkanRenderPass>;
    using VulkanRenderPassSharedPtr                       = std::shared_ptr<VulkanRenderPass>;
    using VulkanSamplerPtr                                = std::unique_ptr<VulkanSampler>;
    using VulkanSamplerSharedPtr                          = std::shared_ptr<VulkanSampler>;
    using VulkanSemaphorePtr                              = std::unique_ptr<VulkanSemaphore>;
    using VulkanSemaphoreSharedPtr                        = std::shared_ptr<VulkanSemaphore>;
    using VulkanShaderBindingTablePtr                     = std::unique_ptr<VulkanShaderBindingTable>;
    using VulkanShaderBindingTableSharedPtr               = std::shared_ptr<VulkanShaderBindingTable>;
    using VulkanShaderModulePtr                           = std::unique_ptr<VulkanShaderModule>;
    using VulkanShaderModuleSharedPtr                     = std::shared_ptr<VulkanShaderModule>;
    using VulkanSubpassPtr                                = std::unique_ptr<VulkanSubpass>;
    using VulkanSubpassSharedPtr                          = std::shared_ptr<VulkanSubpass>;
    using VulkanSurfacePtr                                = std::unique_ptr<VulkanSurface>;
    using VulkanSurfaceSharedPtr                          = std::shared_ptr<VulkanSurface>;
    using VulkanSwapchainPtr                              = std::unique_ptr<VulkanSwapchain>;
    using VulkanSwapchainSharedPtr                        = std::shared_ptr<VulkanSwapchain>;
    using VulkanTimelineSemaphorePtr                      = std::unique_ptr<VulkanTimelineSemaphore>;
    using VulkanTimelineSemaphoreSharedPtr                = std::shared_ptr<VulkanTimelineSemaphore>;
    using VulkanTopLevelAccelerationStructurePtr          = std::unique_ptr<VulkanTopLevelAccelerationStructure>;
    using VulkanTopLevelAccelerationStructureSharedPtr    = std::shared_ptr<VulkanTopLevelAccelerationStructure>;
}  // namespace sol