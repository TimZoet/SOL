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
    class VulkanFence
    {
    public:
        /**
         * \brief VulkanFence settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Create fence in signaled state.
             */
            bool signaled = false;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanFence() = delete;

        VulkanFence(const Settings& set, VkFence vkFence);

        VulkanFence(const VulkanFence&) = delete;

        VulkanFence(VulkanFence&&) = delete;

        ~VulkanFence() noexcept;

        VulkanFence& operator=(const VulkanFence&) = delete;

        VulkanFence& operator=(VulkanFence&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan fence.
         * \param settings Settings.
         * \throws VulkanError Thrown if fence creation failed.
         * \return Vulkan fence.
         */
        [[nodiscard]] static VulkanFencePtr create(const Settings& settings);

        /**
         * \brief Create a list of new Vulkan fences.
         * \param settings Settings.
         * \param count Number of fences to create.
         * \throws VulkanError Thrown if fence creation failed.
         * \return List of Vulkan fences.
         */
        [[nodiscard]] static std::vector<VulkanFencePtr> create(Settings settings, size_t count);

        /**
         * \brief Create a new Vulkan fence.
         * \param settings Settings.
         * \throws VulkanError Thrown if fence creation failed.
         * \return Vulkan fence.
         */
        [[nodiscard]] static VulkanFenceSharedPtr createShared(const Settings& settings);

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
         * \brief Get the fence handle managed by this object.
         * \return Fence handle.
         */
        [[nodiscard]] const VkFence& get() const noexcept;

        /**
         * \brief Returns whether this fence is (going to be) signaled.
         * Note that this does not correspond to the actual state of the VkFence and is set manually.
         * \return True if signaled, false otherwise.
         */
        [[nodiscard]] bool isSignaled() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the signaled state of this fence.
         * Note that this does not correspond to the actual state of the VkFence and waiting on or resetting it through
         * the Vulkan API has no effect on this value.
         * \param value Value.
         */
        void setSignaled(bool value) noexcept;

    private:
        [[nodiscard]] static VkFence createImpl(const Settings& settings);

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        VulkanDevice* device = nullptr;
#endif

        /**
         * \brief Vulkan fence.
         */
        VkFence fence = VK_NULL_HANDLE;

        bool signaled = false;
    };
}  // namespace sol