#include "sol-material/material_instance.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid_system_generator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"
#include "sol-material/material.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MaterialInstance::MaterialInstance() { uuid = uuids::uuid_system_generator{}(); }

    MaterialInstance::~MaterialInstance() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    VulkanDevice& MaterialInstance::getDevice() noexcept { return getMaterial().getDevice(); }

    const VulkanDevice& MaterialInstance::getDevice() const noexcept { return getMaterial().getDevice(); }

    const uuids::uuid& MaterialInstance::getUuid() const noexcept { return uuid; }

    const std::string& MaterialInstance::getName() const noexcept { return name; }

    const VulkanTopLevelAccelerationStructure* MaterialInstance::getAccelerationStructureData(size_t) const
    {
        throw SolError("Not implemented.");
    }

    const Texture2D* MaterialInstance::getCombinedImageSamplerData(size_t) const { throw SolError("Not implemented."); }

    const VulkanBuffer* MaterialInstance::getStorageBufferData(size_t) const { throw SolError("Not implemented."); }

    const Texture2D* MaterialInstance::getStorageImageData(size_t) const { throw SolError("Not implemented."); }

    bool MaterialInstance::isUniformBufferStale(size_t) const { throw SolError("Not implemented."); }

    const void* MaterialInstance::getUniformBufferData(size_t) const { throw SolError("Not implemented."); }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void MaterialInstance::setName(std::string value) { name = std::move(value); }
}  // namespace sol
