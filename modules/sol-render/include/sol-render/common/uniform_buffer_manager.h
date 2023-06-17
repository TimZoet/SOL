#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"
#include "sol-memory/fwd.h"
#include "sol-render/fwd.h"
#include "sol-render/common/uniform_buffer.h"

namespace sol
{
    class UniformBufferManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct UniformBufferReference
        {
            //TODO: Slot in uniformBuffer must be released.
            /**
             * \brief Uniform buffer.
             */
            UniformBuffer* uniformBuffer = nullptr;

            /**
             * \brief Slot index.
             */
            size_t slot = 0;

            /**
             * \brief Offset in buffer in bytes.
             */
            size_t offset = 0;
        };

        using BufferMap = std::unordered_map<const Material*, std::vector<UniformBufferPtr>>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UniformBufferManager() = default;

        UniformBufferManager(MemoryManager& memManager, size_t dataSetCount);

        UniformBufferManager(const UniformBufferManager&) = delete;

        UniformBufferManager(UniformBufferManager&&) = delete;

        ~UniformBufferManager() noexcept;

        UniformBufferManager& operator=(const UniformBufferManager&) = delete;

        UniformBufferManager& operator=(UniformBufferManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        MemoryManager& getMemoryManager() noexcept;

        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        UniformBuffer* create(const Material&                          material,
                              uint32_t                                 setIndex,
                              uint32_t                                 bindingIndex,
                              MaterialLayoutDescription::SharingMethod sharingMethod,
                              size_t                                   slotCount,
                              size_t                                   bufferSize);

        UniformBuffer* getOrCreate(const Material&                          material,
                                   uint32_t                                 setIndex,
                                   uint32_t                                 bindingIndex,
                                   MaterialLayoutDescription::SharingMethod sharingMethod,
                                   size_t                                   slotCount,
                                   size_t                                   bufferSize);

        void mapAll(uint32_t setIndex);

        void unmapAll(uint32_t setIndex);

        void flushAll(uint32_t setIndex);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        BufferMap bufferMap;

        size_t setCount = 0;
    };
}  // namespace sol