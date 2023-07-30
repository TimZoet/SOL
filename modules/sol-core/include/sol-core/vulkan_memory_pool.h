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
    class VulkanMemoryPool
    {
    public:
        /**
         * \brief VulkanMemoryPool settings.
         */
        struct Settings
        {
            ObjectRefSetting<VulkanMemoryAllocator> allocator;

            /**
             * \brief Pool creation flags.
             */
            VmaPoolCreateFlags flags = 0;

            /**
             * \brief Buffer usage. If not 0, this memory pool can be used for buffer allocation.
             */
            VkBufferUsageFlags bufferUsage = 0;

            /**
             * \brief Image usage. If not 0, this memory pool can be used for image allocation.
             */
            VkImageUsageFlags imageUsage = 0;

            /**
             * \brief Memory usage. Defaults to VMA_MEMORY_USAGE_AUTO.
             */
            VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

            /**
             * \brief Size of memory blocks (in bytes).
             */
            size_t blockSize = 0;

            /**
             * \brief Minimum number of blocks.
             */
            size_t minBlocks = 0;

            /**
             * \brief Maximum number of blocks.
             */
            size_t maxBlocks = 1;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanMemoryPool() = delete;

        VulkanMemoryPool(const Settings& set, VmaPool vmaPool);

        VulkanMemoryPool(const VulkanMemoryPool&) = delete;

        VulkanMemoryPool(VulkanMemoryPool&&) = delete;

        ~VulkanMemoryPool() noexcept;

        VulkanMemoryPool& operator=(const VulkanMemoryPool&) = delete;

        VulkanMemoryPool& operator=(VulkanMemoryPool&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan memory pool.
         * \param settings Settings.
         * \throws VulkanError Thrown if pool creation failed.
         * \return Vulkan memory pool.
         */
        [[nodiscard]] static VulkanMemoryPoolPtr create(const Settings& settings);

        /**
         * \brief Create a new Vulkan memory pool.
         * \param settings Settings.
         * \throws VulkanError Thrown if pool creation failed.
         * \return Vulkan memory pool.
         */
        [[nodiscard]] static VulkanMemoryPoolSharedPtr createShared(const Settings& settings);

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
         * \brief Get the pool handle managed by this object.
         * \return Pool handle.
         */
        [[nodiscard]] const VmaPool& get() const noexcept;

    private:
        [[nodiscard]] static VmaPool createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;

        /**
         * \brief Vulkan memory pool.
         */
        VmaPool pool = VK_NULL_HANDLE;
    };
}  // namespace sol