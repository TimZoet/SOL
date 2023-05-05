#pragma once

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>
#include "VulkanMemoryAllocator/vk_mem_alloc.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/object_ref_setting.h"

namespace sol
{
    class VulkanMemoryAllocator
    {
    public:
        /**
         * \brief VulkanMemoryAllocator settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanMemoryAllocator() = delete;

        VulkanMemoryAllocator(SettingsPtr settingsPtr, VmaAllocator vmaAllocator);

        VulkanMemoryAllocator(const VulkanMemoryAllocator&) = delete;

        VulkanMemoryAllocator(VulkanMemoryAllocator&&) = delete;

        ~VulkanMemoryAllocator() noexcept;

        VulkanMemoryAllocator& operator=(const VulkanMemoryAllocator&) = delete;

        VulkanMemoryAllocator& operator=(VulkanMemoryAllocator&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan memory allocator.
         * \param settings Settings.
         * \throws VulkanError Thrown if allocator creation failed.
         * \return Vulkan memory allocator.
         */
        [[nodiscard]] static VulkanMemoryAllocatorPtr create(Settings settings);

        /**
         * \brief Create a new Vulkan memory allocator.
         * \param settings Settings.
         * \throws VulkanError Thrown if allocator creation failed.
         * \return Vulkan memory allocator.
         */
        [[nodiscard]] static VulkanMemoryAllocatorSharedPtr createShared(Settings settings);

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
         * \brief Get the allocator handle managed by this object.
         * \return Allocator handle.
         */
        [[nodiscard]] const VmaAllocator& get() const noexcept;

    private:
        [[nodiscard]] static VmaAllocator createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan memory allocator.
         */
        VmaAllocator allocator = VK_NULL_HANDLE;
    };
}  // namespace sol