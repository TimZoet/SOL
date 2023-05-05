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

        VulkanBuffer() = delete;

        VulkanBuffer(SettingsPtr settingsPtr, VkBuffer vkBuffer, VmaAllocation vmaAllocation, void* pMappedData);

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
         * \throws VulkanError Thrown if buffer creation failed.
         * \return Vulkan buffer.
         */
        [[nodiscard]] static VulkanBufferPtr create(Settings settings);

        /**
         * \brief Create a new Vulkan buffer.
         * \param settings Settings.
         * \throws VulkanError Thrown if buffer creation failed.
         * \return Vulkan buffer.
         */
        [[nodiscard]] static VulkanBufferSharedPtr createShared(Settings settings);

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
        [[nodiscard]] static std::tuple<VkBuffer, VmaAllocation, void*> createImpl(const Settings& settings);

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

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