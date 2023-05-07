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
#include "sol-core/object_ref_setting.h"

namespace sol
{
    class VulkanCommandBufferList
    {
    public:
        /**
         * \brief VulkanCommandBufferList settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan command pool.
             */
            ObjectRefSetting<VulkanCommandPool> commandPool;

            VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

            size_t size = 0;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanCommandBufferList() = delete;

        VulkanCommandBufferList(const Settings& set, std::vector<VkCommandBuffer> commandBuffers);

        VulkanCommandBufferList(const VulkanCommandBufferList&) = delete;

        VulkanCommandBufferList(VulkanCommandBufferList&&) = delete;

        ~VulkanCommandBufferList() noexcept;

        VulkanCommandBufferList& operator=(const VulkanCommandBufferList&) = delete;

        VulkanCommandBufferList& operator=(VulkanCommandBufferList&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan command buffer list.
         * \param settings Settings.
         * \throws VulkanError Thrown if command buffer list creation failed.
         * \return Vulkan command buffer list.
         */
        [[nodiscard]] static VulkanCommandBufferListPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan command buffer list.
         * \param settings Settings.
         * \throws VulkanError Thrown if command buffer list creation failed.
         * \return Vulkan command buffer list.
         */
        [[nodiscard]] static VulkanCommandBufferListSharedPtr createShared(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;
#endif

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
         * \brief Get the command pool.
         * \return VulkanCommandPool.
         */
        [[nodiscard]] VulkanCommandPool& getCommandPool() noexcept;

        /**
         * \brief Get the command pool.
         * \return VulkanCommandPool.
         */
        [[nodiscard]] const VulkanCommandPool& getCommandPool() const noexcept;

        [[nodiscard]] size_t getSize() const noexcept;

        [[nodiscard]] const VkCommandBuffer& get(size_t index) const;

        ////////////////////////////////////////////////////////////////
        // Commands.
        ////////////////////////////////////////////////////////////////

        void beginCommand(size_t index) const;

        void beginOneTimeCommand(size_t index) const;

        void endCommand(size_t index) const;

        void resetCommand(size_t index, VkCommandBufferResetFlags flags) const;

    private:
        [[nodiscard]] static std::vector<VkCommandBuffer> createImpl(const Settings& settings);

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        VulkanCommandPool* commandPool = nullptr;
#endif

        /**
         * \brief Vulkan command buffer list.
         */
        std::vector<VkCommandBuffer> buffers;
    };
}  // namespace sol