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
    class VulkanBuffer
    {
    public:
        /**
         * \brief VulkanBuffer settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Size in bytes.
             */
            size_t size = 0;

            /**
             * \brief Buffer usage.
             */
            VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;

            /**
             * \brief Sharing mode.
             */
            VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            /**
             * \brief Optional allocator. If set, also fill in the properties of the vma member.
             */
            ObjectRefSetting<VulkanMemoryAllocator> allocator;

            /**
             * \brief Settings for the VulkanMemoryAllocator.
             */
            struct
            {
                /**
                 * \brief Optional memory pool.
                 */
                ObjectRefSetting<VulkanMemoryPool> pool;

                /**
                 * \brief Memory usage.
                 */
                VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;

                /**
                 * \brief Required memory property flags.
                 */
                VkMemoryPropertyFlags requiredFlags = 0;

                /**
                 * \brief Preferred memory property flags.
                 */
                VkMemoryPropertyFlags preferredFlags = 0;

                /**
                 * \brief Preferred allocation flags.
                 */
                VmaAllocationCreateFlags flags = 0;

                /**
                 * \brief Optional alignment.
                 */
                VkDeviceSize alignment = 0;
            } vma;
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanBuffer() = delete;

        VulkanBuffer(const Settings& set, VkBuffer vkBuffer, VmaAllocation vmaAllocation, void* pMappedData);

        VulkanBuffer(const VulkanBuffer&) = delete;

        VulkanBuffer(VulkanBuffer&&) = delete;

        ~VulkanBuffer() noexcept;

        VulkanBuffer& operator=(const VulkanBuffer&) = delete;

        VulkanBuffer& operator=(VulkanBuffer&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan buffer.
         * \param settings Settings.
         * \param throwOnOutOfMemory Throw an exception if buffer allocation failed due to an out of memory error.
         * \return Vulkan buffer or nullptr if out of memory and throwOnOutOfMemory is false.
         */
        [[nodiscard]] static VulkanBufferPtr create(const Settings& settings, bool throwOnOutOfMemory = true);

        /**
         * \brief Create a new Vulkan buffer.
         * \param settings Settings.
         * \param throwOnOutOfMemory Throw an exception if buffer allocation failed due to an out of memory error.
         * \return Vulkan buffer or nullptr if out of memory and throwOnOutOfMemory is false.
         */
        [[nodiscard]] static VulkanBufferSharedPtr createShared(const Settings& settings,
                                                                bool            throwOnOutOfMemory = true);

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
         * \brief Get the size of the buffer.
         * \return Size in bytes.
         */
        [[nodiscard]] size_t getSize() const noexcept;

        /**
         * \brief Get the device address of this buffer.
         * \return VkDeviceAddress.
         */
        [[nodiscard]] VkDeviceAddress getDeviceAddress() const noexcept;

        /**
         * \brief Whether this buffer has a custom allocator.
         * \return True or false.
         */
        [[nodiscard]] bool hasAllocator() const noexcept;

        /**
         * \brief Get the allocator.
         * \return VulkanMemoryAllocator.
         */
        [[nodiscard]] VulkanMemoryAllocator& getAllocator() noexcept;

        /**
         * \brief Get the allocator.
         * \return VulkanMemoryAllocator.
         */
        [[nodiscard]] const VulkanMemoryAllocator& getAllocator() const noexcept;

        /**
         * \brief Get the buffer handle managed by this object.
         * \return Buffer handle.
         */
        [[nodiscard]] const VkBuffer& get() const noexcept;

        /**
         * \brief Get the memory requirements of this buffer.
         * \return Memory requirements.
         */
        [[nodiscard]] const VkMemoryRequirements& getMemoryRequirements() const noexcept;

        [[nodiscard]] bool isMapped() const noexcept;

        template<typename T>
        [[nodiscard]] T* getMappedData() const noexcept
        {
            return static_cast<T*>(mappedData);
        }

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void bindMemory(VulkanDeviceMemory& memory, size_t offset);

        /**
         * \brief Set the buffer data from a contiguous data array. size should not exceed total size of buffer.
         * \param data Data array. Should contain at least size bytes.
         * \param size Number of bytes to set.
         */
        void setData(const void* data, size_t size) const;

        /**
         * \brief Set the buffer data from a contiguous data array. size + offset should not exceed total size of buffer.
         * \param data Data array. Should contain at least size bytes.
         * \param size Number of bytes to set. 
         * \param offset Offset in bytes at which to start.
         */
        void setData(const void* data, size_t size, size_t offset) const;

        /**
         * \brief Set the buffer data by repeating a single value. size * count should not exceed total size of buffer.
         * \param data Pointer to value. Should contain at least size bytes.
         * \param size Size of the value.
         * \param count Number of times to repeat the value.
         */
        void setDataRepeated(const void* data, size_t size, size_t count) const;

        /**
         * \brief Set the buffer data by repeating a single value. size * count + offset should not exceed total size of buffer.
         * \param data Pointer to value. Should contain at least size bytes.
         * \param size Size of the value.
         * \param count Number of times to repeat the value.
         * \param offset Offset in bytes at which to start.
         */
        void setDataRepeated(const void* data, size_t size, size_t count, size_t offset) const;

        /**
         * \brief Set the buffer data by treating both the buffer memory and the data array as 2D arrays. regionOffset + regionSize should not exceed totalSize.
         * \param data Data array. Should contain at least as much data as elementSize * regionSize.x * regionSize.y, where elementSize = buffer.size / (totalSize.x * totalSize.y).
         * \param totalSize Assumed size/resolution of the data in this buffer. Size of this buffer in bytes should be divisible by totalSize.x * totalSize.y.
         * \param regionOffset Offset of the region within the totalSize to fill.
         * \param regionSize Size of the region to fill.
         */
        void setData2D(const void*             data,
                       std::array<uint32_t, 2> totalSize,
                       std::array<uint32_t, 2> regionOffset,
                       std::array<uint32_t, 2> regionSize) const;

        /**
         * \brief Set the buffer data by repeating a single value while treating the buffer memory as a 2D array. regionOffset + regionSize should not exceed totalSize.
         * \param data Pointer to value. Should contain at least as much data as buffer.size / (totalSize.x * totalSize.y).
         * \param totalSize Assumed size/resolution of the data in this buffer. Size of this buffer in bytes should be divisible by totalSize.x * totalSize.y.
         * \param regionOffset Offset of the region within the totalSize to fill.
         * \param regionSize Size of the region to fill.
         */
        void setDataRepeated2D(const void*             data,
                               std::array<uint32_t, 2> totalSize,
                               std::array<uint32_t, 2> regionOffset,
                               std::array<uint32_t, 2> regionSize) const;

        void map();

        void unmap();

        void flush() const;

    private:
        [[nodiscard]] static std::tuple<VkBuffer, VmaAllocation, void*> createImpl(const Settings& settings,
                                                                                   bool            throwOnOutOfMemory);

#ifdef SOL_CORE_ENABLE_CACHE_SETTINGS
        /**
         * \brief Settings with which this object was created.
         */
        Settings settings;
#else
        /**
         * \brief Vulkan device.
         */
        VulkanDevice* device = nullptr;

        /**
         * \brief Size in bytes.
         */
        size_t size = 0;

        /**
         * \brief Optional allocator.
         */
        VulkanMemoryAllocator* allocator = nullptr;
#endif

        /**
         * \brief Vulkan buffer.
         */
        VkBuffer buffer = VK_NULL_HANDLE;

        /**
         * \brief Memory requirements of the buffer.
         */
        VkMemoryRequirements memoryRequirements;

        VulkanDeviceMemory* deviceMemory = nullptr;

        VmaAllocation allocation = VK_NULL_HANDLE;

        void* mappedData = nullptr;
    };
}  // namespace sol