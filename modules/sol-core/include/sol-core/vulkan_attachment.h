#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

namespace sol
{
    class VulkanAttachment
    {
    public:
        friend class VulkanRenderPassLayout;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanAttachment() = delete;

        VulkanAttachment(VulkanRenderPassLayout& rpLayout, uint32_t attachmentIndex);

        VulkanAttachment(const VulkanAttachment& other);

        VulkanAttachment(VulkanAttachment&&) = delete;

        ~VulkanAttachment() noexcept;

        VulkanAttachment& operator=(const VulkanAttachment& other);

        VulkanAttachment& operator=(VulkanAttachment&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the layout.
         * \return VulkanRenderPassLayout.
         */
        [[nodiscard]] VulkanRenderPassLayout& getLayout() noexcept;

        /**
         * \brief Get the layout.
         * \return VulkanRenderPassLayout.
         */
        [[nodiscard]] const VulkanRenderPassLayout& getLayout() const noexcept;

        /**
         * \brief Get the index of this attachment in the render pass.
         * \return Index.
         */
        [[nodiscard]] uint32_t getIndex() const noexcept;

        [[nodiscard]] VkAttachmentDescriptionFlags getDescriptionFlags() const noexcept;

        [[nodiscard]] VkFormat getFormat() const noexcept;

        [[nodiscard]] VkSampleCountFlagBits getSamples() const noexcept;

        [[nodiscard]] VkAttachmentLoadOp getLoadOp() const noexcept;

        [[nodiscard]] VkAttachmentStoreOp getStoreOp() const noexcept;

        [[nodiscard]] VkAttachmentLoadOp getStencilLoadOp() const noexcept;

        [[nodiscard]] VkAttachmentStoreOp getStencilStoreOp() const noexcept;

        [[nodiscard]] VkImageLayout getInitialLayout() const noexcept;

        [[nodiscard]] VkImageLayout getFinalLayout() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setDescriptionFlags(VkAttachmentDescriptionFlags descriptionFlags);

        void setFormat(VkFormat f);

        void setSamples(VkSampleCountFlagBits s);

        void setLoadOp(VkAttachmentLoadOp op);

        void setStoreOp(VkAttachmentStoreOp op);

        void setStencilLoadOp(VkAttachmentLoadOp op);

        void setStencilStoreOp(VkAttachmentStoreOp op);

        void setInitialLayout(VkImageLayout imageLayout);

        void setFinalLayout(VkImageLayout imageLayout);

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Destroy this attachment, removing it from the render pass layout. Instantly invalidates all references to this object.
         */
        void destroy() const;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VulkanRenderPassLayout* layout = nullptr;

        uint32_t index = 0;

        VkAttachmentDescriptionFlags flags = 0;

        VkFormat format = VK_FORMAT_UNDEFINED;

        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

        VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

        VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

        VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    };
}  // namespace sol