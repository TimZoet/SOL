#include "sol-mesh/i_mesh.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/mesh_description.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    IMesh::IMesh() = default;

    IMesh::IMesh(MeshManager& manager, const uuids::uuid id) : meshManager(&manager), uuid(id) {}

    IMesh::~IMesh() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    MeshManager& IMesh::getMeshManager() noexcept { return *meshManager; }

    const MeshManager& IMesh::getMeshManager() const noexcept { return *meshManager; }

    const uuids::uuid& IMesh::getUuid() const noexcept { return uuid; }

    const std::string& IMesh::getName() const noexcept { return name; }

    const VulkanQueueFamily* IMesh::getCurrentFamily() const noexcept { return currentFamily; }

    const VulkanQueueFamily* IMesh::getTargetFamily() const noexcept { return targetFamily; }

    VkPipelineStageFlags IMesh::getStageFlags() const noexcept { return stageFlags; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void IMesh::setName(std::string value) { name = std::move(value); }

    void IMesh::setCurrentFamily(const VulkanQueueFamily& family) { currentFamily = &family; }

    void IMesh::setTargetFamily(const VulkanQueueFamily& family) { targetFamily = &family; }

    void IMesh::setStageFlags(const VkPipelineStageFlags flags) { stageFlags = flags; }

    ////////////////////////////////////////////////////////////////
    // Update.
    ////////////////////////////////////////////////////////////////

    void IMesh::update(MeshDescriptionPtr) { throw SolError("Updating this mesh type is not supported."); }

}  // namespace sol
