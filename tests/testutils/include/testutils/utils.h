#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-memory/fwd.h"

class BasicFixture
{
public:
    BasicFixture();

    virtual ~BasicFixture() noexcept;

    [[nodiscard]] static sol::VulkanInstance& getInstance();

    [[nodiscard]] static sol::VulkanPhysicalDevice& getPhysicalDevice();

    [[nodiscard]] static sol::VulkanDevice& getDevice();
};

class MemoryManagerFixture : public BasicFixture
{
public:
    MemoryManagerFixture();

    ~MemoryManagerFixture() noexcept override;

    [[nodiscard]] static sol::MemoryManager& getMemoryManager();

    [[nodiscard]] static sol::TransferManager& getTransferManager();
};

class ImageDataGeneration
{
public:
    /**
     * \brief 8 bits per channel. RGBA. 256x256. Filled with a gradient.
     * \return 
     */
    [[nodiscard]] static std::vector<uint32_t> genR8G8B8A8W256H256Gradient();
};
