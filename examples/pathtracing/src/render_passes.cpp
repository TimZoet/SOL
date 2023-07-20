#include "pathtracing/render_passes.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_attachment.h"
#include "sol-core/vulkan_render_pass_layout.h"
#include "sol-core/vulkan_subpass.h"

sol::VulkanRenderPassPtr createGuiRenderPass(sol::VulkanDevice& device, const VkFormat colorFormat)
{
    sol::VulkanRenderPassLayout renderPassLayout;
    auto&                       colorAttachment = renderPassLayout.createAttachment();
    colorAttachment.setFormat(colorFormat);
    colorAttachment.setSamples(VK_SAMPLE_COUNT_1_BIT);
    colorAttachment.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR);
    colorAttachment.setStoreOp(VK_ATTACHMENT_STORE_OP_STORE);
    colorAttachment.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    colorAttachment.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE);
    colorAttachment.setInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
    colorAttachment.setFinalLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    auto& subpass = renderPassLayout.createSubpass();
    subpass.setPipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS);
    subpass.addColorAttachment(colorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    subpass.addExternalDependency(true,
                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                  0,
                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                  0);

    renderPassLayout.finalize();

    sol::VulkanRenderPass::Settings renderPassSettings;
    renderPassSettings.device = device;
    renderPassSettings.layout = &renderPassLayout;
    return sol::VulkanRenderPass::create(renderPassSettings);
}


sol::VulkanRenderPassPtr
  createViewerRenderPass(sol::VulkanDevice& device, const VkFormat colorFormat, const VkFormat depthFormat)
{
    sol::VulkanRenderPassLayout renderPassLayout;
    auto&                       colorAttachment = renderPassLayout.createAttachment();
    colorAttachment.setFormat(colorFormat);
    colorAttachment.setSamples(VK_SAMPLE_COUNT_1_BIT);
    colorAttachment.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR);
    colorAttachment.setStoreOp(VK_ATTACHMENT_STORE_OP_STORE);
    colorAttachment.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    colorAttachment.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE);
    colorAttachment.setInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
    colorAttachment.setFinalLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    auto& depthAttachment = renderPassLayout.createAttachment();
    depthAttachment.setFormat(depthFormat);
    depthAttachment.setSamples(VK_SAMPLE_COUNT_1_BIT);
    depthAttachment.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR);
    depthAttachment.setStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE);
    depthAttachment.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    depthAttachment.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE);
    depthAttachment.setInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
    depthAttachment.setFinalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    auto& subpass = renderPassLayout.createSubpass();
    subpass.setPipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS);
    subpass.addColorAttachment(colorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    subpass.setDepthStencilAttachment(depthAttachment, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    subpass.addExternalDependency(
      true,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      0,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
      0);

    renderPassLayout.finalize();

    sol::VulkanRenderPass::Settings renderPassSettings;
    renderPassSettings.device = device;
    renderPassSettings.layout = &renderPassLayout;
    return sol::VulkanRenderPass::create(renderPassSettings);
}

sol::VulkanRenderPassPtr createDisplayRenderPass(sol::VulkanDevice& device, const VkFormat colorFormat)
{
    sol::VulkanRenderPassLayout renderPassLayout;
    auto&                       colorAttachment = renderPassLayout.createAttachment();
    colorAttachment.setFormat(colorFormat);
    colorAttachment.setSamples(VK_SAMPLE_COUNT_1_BIT);
    colorAttachment.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR);
    colorAttachment.setStoreOp(VK_ATTACHMENT_STORE_OP_STORE);
    colorAttachment.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    colorAttachment.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE);
    colorAttachment.setInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
    colorAttachment.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    auto& subpass = renderPassLayout.createSubpass();
    subpass.setPipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS);
    subpass.addColorAttachment(colorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    subpass.addExternalDependency(true,
                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                  0,
                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                  0);

    renderPassLayout.finalize();

    sol::VulkanRenderPass::Settings renderPassSettings;
    renderPassSettings.device = device;
    renderPassSettings.layout = &renderPassLayout;
    return sol::VulkanRenderPass::create(renderPassSettings);
}