#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

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
    class VulkanRenderPassLayout
    {
    public:
        friend class VulkanAttachment;
        friend class VulkanSubpass;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanRenderPassLayout();

        VulkanRenderPassLayout(const VulkanRenderPassLayout& other);

        VulkanRenderPassLayout(VulkanRenderPassLayout&&) = delete;

        ~VulkanRenderPassLayout() noexcept;

        VulkanRenderPassLayout& operator=(const VulkanRenderPassLayout& other);

        VulkanRenderPassLayout& operator=(VulkanRenderPassLayout&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool isFinalized() const noexcept;

        [[nodiscard]] const std::vector<VulkanAttachmentPtr>& getAttachments() const noexcept;

        [[nodiscard]] const std::vector<VulkanSubpassPtr>& getSubpasses() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Modifiers.
        ////////////////////////////////////////////////////////////////

        void finalize();

        void requireFinalized() const;

        void requireNonFinalized() const;

        /**
         * \brief Create a new attachment in this render pass.
         * \return Attachment.
         */
        VulkanAttachment& createAttachment();

        /**
         * \brief Create a new subpass in this render pass.
         * \return Subpass.
         */
        VulkanSubpass& createSubpass();

    private:
        void destroyAttachment(uint32_t index);

        void destroySubpass(uint32_t index);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief List of attachments.
         */
        std::vector<VulkanAttachmentPtr> attachments;

        /**
         * \brief List of subpasses.
         */
        std::vector<VulkanSubpassPtr> subpasses;

        bool finalized = false;
    };
}  // namespace sol