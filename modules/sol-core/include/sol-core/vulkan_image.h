#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/object_ref_setting.h"

namespace sol
{
    class VulkanImage
    {
    public:
        /**
         * \brief VulkanImage settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Indicates this VulkanImage holds a non-owning reference to a VkImage from a swapchain.
             * If this is true, all other settings are invalid and should not be accessed.
             */
            bool isSwapchainImage = false;

            VkFormat format = VK_FORMAT_UNDEFINED;

            uint32_t width = 0;

            uint32_t height = 0;

            uint32_t depth = 0;

            VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;

            VkImageUsageFlags imageUsage = 0;

            VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            /**
             * \brief Optional allocator.
             */
            ObjectRefSetting<VulkanMemoryAllocator> allocator;

            /**
             * \brief Memory usage. Only used when allocator is specified.
             */
            VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;

            /**
             * \brief Required memory property flags. Only used when allocator is specified.
             */
            VkMemoryPropertyFlags requiredFlags = 0;

            /**
             * \brief Preferred memory property flags. Only used when allocator is specified.
             */
            VkMemoryPropertyFlags preferredFlags = 0;

            /**
             * \brief Preferred allocation flags. Only used when allocator is specified.
             */
            VmaAllocationCreateFlags flags = 0;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanImage() = delete;

        VulkanImage(SettingsPtr settingsPtr, VkImage vkImage, VmaAllocation vmaAllocation);

        VulkanImage(const VulkanImage&) = delete;

        VulkanImage(VulkanImage&&) = delete;

        ~VulkanImage() noexcept;

        VulkanImage& operator=(const VulkanImage&) = delete;

        VulkanImage& operator=(VulkanImage&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan image.
         * \param settings Settings.
         * \throws VulkanError Thrown if image creation failed.
         * \return Vulkan image.
         */
        [[nodiscard]] static VulkanImagePtr create(Settings settings);

        /**
         * \brief Create a new Vulkan image.
         * \param settings Settings.
         * \throws VulkanError Thrown if image creation failed.
         * \return Vulkan image.
         */
        [[nodiscard]] static VulkanImageSharedPtr createShared(Settings settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        /**
         * \brief Get the image handle managed by this object.
         * \return Image handle.
         */
        [[nodiscard]] const VkImage& get() const noexcept;

        [[nodiscard]] const VkMemoryRequirements& getMemoryRequirements() const noexcept;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void bindMemory(VulkanDeviceMemory& memory, size_t offset);

    private:
        [[nodiscard]] static std::pair<VkImage, VmaAllocation> createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan image.
         */
        VkImage image = VK_NULL_HANDLE;

        /**
         * \brief Memory requirements of the image buffer.
         */
        VkMemoryRequirements memoryRequirements;

        VulkanDeviceMemory* deviceMemory = nullptr;

        VmaAllocation allocation = VK_NULL_HANDLE;
    };
}  // namespace sol