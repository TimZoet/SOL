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

    IImage::IImage(MemoryManager& memoryManager) : manager(&memoryManager) {}

    IImage::~IImage() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& IImage::getDevice() noexcept { return manager->getDevice(); }

    const VulkanDevice& IImage::getDevice() const noexcept { return manager->getDevice(); }

    MemoryManager& IImage::getMemoryManager() noexcept { return *manager; }

    const MemoryManager& IImage::getMemoryManager() const noexcept { return *manager; }

}  // namespace sol
