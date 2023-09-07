#include "sol-memory/i_image.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/memory_manager.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    IImage::IImage(MemoryManager& memoryManager, uuids::uuid id) : manager(&memoryManager), uuid(id) {}

    IImage::~IImage() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& IImage::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& IImage::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& IImage::getMemoryManager() noexcept { return *manager; }

    const MemoryManager& IImage::getMemoryManager() const noexcept { return *manager; }

    const uuids::uuid& IImage::getUuid() const noexcept { return uuid; }

    uint32_t IImage::getWidth() const noexcept { return getSize()[0]; }

    uint32_t IImage::getHeight() const noexcept { return getSize()[1]; }

    uint32_t IImage::getDepth() const noexcept { return getSize()[2]; }
}  // namespace sol
