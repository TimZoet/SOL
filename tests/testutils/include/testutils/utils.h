#pragma once

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
