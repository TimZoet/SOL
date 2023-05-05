#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>
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
    class VulkanSubpass
    {
    public:
        friend class VulkanRenderPassLayout;

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct AttachmentReference
        {
            VulkanAttachment* attachment = nullptr;

            VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;
        };

        struct Dependency
        {
            VulkanSubpass* subpass = nullptr;

            bool externalIsSrc = false;

            VkPipelineStageFlags srcStageMask = 0;

            VkPipelineStageFlags dstStageMask = 0;

            VkAccessFlags srcAccessMask = 0;

            VkAccessFlags dstAccessMask = 0;

            VkDependencyFlags dependencyFlags = 0;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanSubpass() = delete;

        VulkanSubpass(VulkanRenderPassLayout& rpLayout, uint32_t subpassIndex);

        VulkanSubpass(const VulkanSubpass& other);

        VulkanSubpass(VulkanSubpass&&) = delete;

        ~VulkanSubpass() noexcept;

        VulkanSubpass& operator=(const VulkanSubpass& other);

        VulkanSubpass& operator=(VulkanSubpass&&) = delete;

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
         * \brief Get the index of this subpass in the render pass.
         * \return Index.
         */
        [[nodiscard]] uint32_t getIndex() const noexcept;

        /**
         * \brief Get description flags.
         * \return VkSubpassDescriptionFlags.
         */
        [[nodiscard]] VkSubpassDescriptionFlags getDescriptionFlags() const noexcept;

        /**
         * \brief Get pipeline bind point.
         * \return VkPipelineBindPoint.
         */
        [[nodiscard]] VkPipelineBindPoint getPipelineBindPoint() const noexcept;

        /**
         * \brief Get the total number of attachments in this subpass, not including preserve attachments.
         * \return Number of attachments.
         */
        [[nodiscard]] size_t getTotalAttachmentCount() const noexcept;

        /**
         * \brief Get the list of input attachments.
         * \return Input attachments.
         */
        [[nodiscard]] const std::vector<AttachmentReference>& getInputAttachments() const noexcept;

        /**
         * \brief Get the list of color attachments.
         * \return Color attachments.
         */
        [[nodiscard]] const std::vector<AttachmentReference>& getColorAttachments() const noexcept;

        /**
         * \brief Get the list of resolve attachments.
         * \return Resolve attachments.
         */
        [[nodiscard]] const std::vector<AttachmentReference>& getResolveAttachments() const noexcept;

        /**
         * \brief Get the (optional) depth stencil attachment.
         * \return Depth stencil attachment.
         */
        [[nodiscard]] const std::optional<AttachmentReference>& getDepthStencilAttachment() const noexcept;

        /**
         * \brief Get the list of preserve attachments.
         * \return Preserve attachments.
         */
        [[nodiscard]] const std::vector<VulkanAttachment*>& getPreserveAttachments() const noexcept;

        /**
         * \brief Get the list of dependencies.
         * \return Dependencies.
         */
        [[nodiscard]] const std::vector<Dependency>& getDependencies() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setDescriptionFlags(VkSubpassDescriptionFlags descriptionFlags);

        void setPipelineBindPoint(VkPipelineBindPoint bindPoint);

        void addInputAttachment(VulkanAttachment& attachment, VkImageLayout imageLayout);

        void addColorAttachment(VulkanAttachment& attachment, VkImageLayout imageLayout);

        void addResolveAttachment(VulkanAttachment& attachment, VkImageLayout imageLayout);

        void setDepthStencilAttachment(VulkanAttachment& attachment, VkImageLayout imageLayout);

        void addPreserveAttachment(VulkanAttachment& attachment);

        /**
         * \brief Add a new dependency between this subpass and an external subpass.
         * \param externalIsSrc If true, external subpass is used as the source. Otherwise, it is used as the destination.
         * \param srcStageMask Source stage mask.
         * \param srcAccessMask Source access mask.
         * \param dstStageMask Destination stage mask.
         * \param dstAccessMask Destination access mask.
         * \param dependencyFlags Dependency flags.
         */
        void addExternalDependency(bool                 externalIsSrc,
                                   VkPipelineStageFlags srcStageMask,
                                   VkAccessFlags        srcAccessMask,
                                   VkPipelineStageFlags dstStageMask,
                                   VkAccessFlags        dstAccessMask,
                                   VkDependencyFlags    dependencyFlags);

        /**
         * \brief Add a new dependency between this subpass, which is the destination, and the given source subpass.
         * \param subpass Source subpass.
         * \param srcStageMask Source stage mask.
         * \param srcAccessMask Source access mask.
         * \param dstStageMask Destination stage mask.
         * \param dstAccessMask Destination access mask.
         * \param dependencyFlags Dependency flags.
         */
        void addDependency(VulkanSubpass&       subpass,
                           VkPipelineStageFlags srcStageMask,
                           VkAccessFlags        srcAccessMask,
                           VkPipelineStageFlags dstStageMask,
                           VkAccessFlags        dstAccessMask,
                           VkDependencyFlags    dependencyFlags);

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Destroy this subpass, removing it from the render pass layout. Instantly invalidated all references to this object.
         */
        void destroy() const;

    private:
        void requireSameLayout(const VulkanSubpass& subpass) const;

        void requireSameLayout(const VulkanAttachment& attachment) const;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Render pass layout this subpass is in.
         */
        VulkanRenderPassLayout* layout = nullptr;

        /**
         * \brief Index of this subpass in render pass.
         */
        uint32_t index = 0;

        /**
         * \brief Description flags.
         */
        VkSubpassDescriptionFlags flags = 0;

        /**
         * \brief Pipeline bind point.
         */
        VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;

        /**
         * \brief Input attachment references.
         */
        std::vector<AttachmentReference> inputAttachments;

        /**
         * \brief Color attachment references.
         */
        std::vector<AttachmentReference> colorAttachments;

        /**
         * \brief Resolve attachment references.
         */
        std::vector<AttachmentReference> resolveAttachments;

        /**
         * \brief DepthStencil attachment references.
         */
        std::optional<AttachmentReference> depthStencilAttachment;

        /**
         * \brief Preserve attachments.
         */
        std::vector<VulkanAttachment*> preserveAttachments;

        /**
         * \brief Subpass dependencies.
         */
        std::vector<Dependency> dependencies;
    };
}  // namespace sol