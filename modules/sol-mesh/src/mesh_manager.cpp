#include "sol-mesh/mesh_manager.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid_system_generator.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_buffer.h"
#include "sol-core/vulkan_command_buffer.h"
#include "sol-core/vulkan_queue.h"
#include "sol-error/sol_error.h"
#include "sol-memory/memory_manager.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/flat_mesh.h"
#include "sol-mesh/indexed_mesh.h"
#include "sol-mesh/mesh_description.h"
#include "sol-mesh/multi_mesh.h"
#include "sol-mesh/shared_mesh.h"
#include "sol-mesh/mesh_layout.h"
#include "sol-mesh/mesh_transfer/default_mesh_transfer.h"
#include "sol-mesh/mesh_transfer/i_mesh_transfer.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    MeshManager::MeshManager() = default;

    MeshManager::MeshManager(MemoryManager& memManager) :
        memoryManager(&memManager), meshTransfer(std::make_unique<DefaultMeshTransfer>(*this))
    {
    }

    MeshManager::MeshManager(MemoryManager& memManager, IMeshTransferPtr mshTransfer) :
        memoryManager(&memManager), meshTransfer(std::move(mshTransfer))
    {
    }

    MeshManager::~MeshManager() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    MemoryManager& MeshManager::getMemoryManager() noexcept { return *memoryManager; }

    const MemoryManager& MeshManager::getMemoryManager() const noexcept { return *memoryManager; }

    ////////////////////////////////////////////////////////////////
    // Layouts.
    ////////////////////////////////////////////////////////////////

    MeshLayout& MeshManager::createMeshLayout(std::string name)
    {
        auto id                      = generateUuid();
        const auto [layout, created] = meshLayouts.try_emplace(id, std::make_unique<MeshLayout>(*this, id));


        if (!created) throw SolError("UUID collision.");

        layout->second->setName(std::move(name));

        return *layout->second;
    }

    MeshLayout& MeshManager::getMeshLayout(const uuids::uuid id) const
    {
        const auto it = meshLayouts.find(id);

        if (it == meshLayouts.end()) throw SolError("Cannot get mesh layout. A layout with this name does not exist.");

        return *it->second;
    }

    bool MeshManager::destroyMeshLayout(const uuids::uuid id) { return meshLayouts.erase(id); }

    ////////////////////////////////////////////////////////////////
    // Meshes.
    ////////////////////////////////////////////////////////////////

    MeshDescriptionPtr MeshManager::createMeshDescription() { return std::make_unique<MeshDescription>(*this); }

    //FlatMesh& MeshManager::createFlatMesh() { return createFlatMeshImpl(); }

    FlatMesh& MeshManager::createFlatMesh(MeshDescriptionPtr meshDescription)
    {
        // Validate MeshDescription.
        if (&meshDescription->getMeshManager() != this)
            throw SolError("Cannot create FlatMesh from MeshDescription: MeshDescription has a different MeshManager.");
        if (meshDescription->getVertexBufferCount() == 0 || meshDescription->getVertexCount(0) == 0 ||
            meshDescription->getVertexSize(0) == 0)
            throw SolError("Cannot create FlatMesh from MeshDescription: missing valid vertex buffer.");

        auto& mesh = createFlatMeshImpl();

        // Create vertex buffer.
        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device = memoryManager->getDevice();
        bufferSettings.size   = meshDescription->getVertexCount(0) * meshDescription->getVertexSize(0);
        bufferSettings.bufferUsage =
          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | meshDescription->getVertexFlags(0);
        bufferSettings.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferSettings.allocator   = memoryManager->getAllocator();
        bufferSettings.memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        bufferSettings.flags       = 0;
        mesh.setVertexBuffer(VulkanBuffer::create(bufferSettings));
        mesh.setVertexCount(meshDescription->getVertexCount(0));

        // Stage transfer.
        meshTransfer->stageCopy(std::move(meshDescription), mesh);

        return mesh;
    }

    void MeshManager::updateFlatMesh(FlatMesh& mesh, MeshDescriptionPtr meshDescription)
    {
        // Validate MeshDescription.
        if (&meshDescription->getMeshManager() != this)
            throw SolError("Cannot update FlatMesh from MeshDescription: MeshDescription has a different MeshManager.");
        if (meshDescription->getVertexBufferCount() == 0 || meshDescription->getVertexCount(0) == 0 ||
            meshDescription->getVertexSize(0) == 0)
            throw SolError("Cannot update FlatMesh from MeshDescription: missing valid vertex buffer.");

        if (&mesh.getMeshManager() != this)
            throw SolError("Cannot update FlatMesh from MeshDescription: Mesh has a different MeshManager.");

        meshTransfer->stageCopy(std::move(meshDescription), mesh);
    }

    FlatMesh& MeshManager::createFlatMeshImpl()
    {
        auto  id      = generateUuid();
        auto  mesh    = std::make_unique<FlatMesh>(*this, id);
        auto& meshRef = *mesh;

        // Make graphics queue the default target family.
        mesh->setTargetFamily(memoryManager->getGraphicsQueue().getFamily());

        assert(meshes.try_emplace(id, std::move(mesh)).second);

        return meshRef;
    }

    IndexedMesh& MeshManager::createIndexedMesh(MeshDescriptionPtr meshDescription)
    {
        // Validate MeshDescription.
        if (&meshDescription->getMeshManager() != this)
            throw SolError(
              "Cannot create IndexedMesh from MeshDescription: MeshDescription has a different MeshManager.");
        if (meshDescription->getVertexBufferCount() == 0 || meshDescription->getVertexCount(0) == 0 ||
            meshDescription->getVertexSize(0) == 0)
            throw SolError("Cannot create IndexedMesh from MeshDescription: missing valid vertex buffer.");
        if (!meshDescription->isIndexed() || meshDescription->getIndexCount() == 0 ||
            meshDescription->getIndexSize() == 0)
            throw SolError("Cannot create IndexedMesh from MeshDescription: missing valid index buffer.");
        (void)meshDescription->getIndexType();

        auto& mesh = createIndexedMeshImpl();

        // Create vertex buffer.
        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device = memoryManager->getDevice();
        bufferSettings.size   = meshDescription->getVertexCount(0) * meshDescription->getVertexSize(0);
        bufferSettings.bufferUsage =
          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | meshDescription->getVertexFlags(0);
        bufferSettings.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferSettings.allocator   = memoryManager->getAllocator();
        bufferSettings.memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        bufferSettings.flags       = 0;
        mesh.setVertexBuffer(VulkanBuffer::create(bufferSettings));
        mesh.setVertexCount(meshDescription->getVertexCount(0));

        // Create index buffer.
        bufferSettings.size = meshDescription->getIndexCount() * meshDescription->getIndexSize();
        bufferSettings.bufferUsage =
          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | meshDescription->getIndexFlags();
        mesh.setIndexBuffer(VulkanBuffer::create(bufferSettings));
        mesh.setIndexCount(meshDescription->getIndexCount());
        mesh.setIndexType(meshDescription->getIndexType());

        // Stage transfer.
        meshTransfer->stageCopy(std::move(meshDescription), mesh);

        return mesh;
    }

    void MeshManager::updateIndexedMesh(IndexedMesh& mesh, MeshDescriptionPtr meshDescription)
    {
        // Validate MeshDescription.
        if (&meshDescription->getMeshManager() != this)
            throw SolError(
              "Cannot update IndexedMesh from MeshDescription: MeshDescription has a different MeshManager.");
        if (meshDescription->getVertexBufferCount() == 0 || meshDescription->getVertexCount(0) == 0 ||
            meshDescription->getVertexSize(0) == 0)
            throw SolError("Cannot update IndexedMesh from MeshDescription: missing valid vertex buffer.");
        if (!meshDescription->isIndexed() || meshDescription->getIndexCount() == 0 ||
            meshDescription->getIndexSize() == 0)
            throw SolError("Cannot update IndexedMesh from MeshDescription: missing valid index buffer.");

        if (&mesh.getMeshManager() != this)
            throw SolError("Cannot update IndexedMesh from MeshDescription: Mesh has a different MeshManager.");

        meshTransfer->stageCopy(std::move(meshDescription), mesh);
    }

    IndexedMesh& MeshManager::createIndexedMeshImpl()
    {
        auto  id      = generateUuid();
        auto  mesh    = std::make_unique<IndexedMesh>(*this, id);
        auto& meshRef = *mesh;

        // Make graphics queue the default target family.
        mesh->setTargetFamily(memoryManager->getGraphicsQueue().getFamily());

        assert(meshes.try_emplace(id, std::move(mesh)).second);

        return meshRef;
    }

    MultiMesh& MeshManager::createMultiMesh(MeshDescriptionPtr meshDescription)
    {
        // Validate MeshDescription.
        if (&meshDescription->getMeshManager() != this)
            throw SolError(
              "Cannot create MultiMesh from MeshDescription: MeshDescription has a different MeshManager.");
        if (meshDescription->getVertexBufferCount() == 0)
            throw SolError("Cannot create MultiMesh from MeshDescription: missing valid vertex buffer.");
        for (size_t i = 0; i < meshDescription->getVertexBufferCount(); i++)
            if (meshDescription->getVertexCount(i) == 0 || meshDescription->getVertexSize(i) == 0)
                throw SolError("Cannot create MultiMesh from MeshDescription: invalid vertex buffer.");
        if (meshDescription->isIndexed() &&
            (meshDescription->getIndexCount() == 0 || meshDescription->getIndexSize() == 0))
            throw SolError("Cannot create MultiMesh from MeshDescription: invalid index buffer.");
        if (meshDescription->isIndexed()) (void)meshDescription->getIndexType();

        auto& mesh = createMultiMeshImpl();
        mesh.setVertexCount(meshDescription->getVertexCount(0));

        // Create vertex buffers.
        VulkanBuffer::Settings bufferSettings;
        bufferSettings.device      = memoryManager->getDevice();
        bufferSettings.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferSettings.allocator   = memoryManager->getAllocator();
        bufferSettings.memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        bufferSettings.flags       = 0;
        for (size_t i = 0; i < meshDescription->getVertexBufferCount(); i++)
        {
            bufferSettings.bufferUsage =
              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | meshDescription->getVertexFlags(i);
            bufferSettings.size = meshDescription->getVertexCount(i) * meshDescription->getVertexSize(i);
            mesh.addVertexBuffer(VulkanBuffer::create(bufferSettings));
        }

        // Create index buffer.
        if (meshDescription->isIndexed())
        {
            bufferSettings.size = meshDescription->getIndexCount() * meshDescription->getIndexSize();
            bufferSettings.bufferUsage =
              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | meshDescription->getIndexFlags();
            mesh.setIndexBuffer(VulkanBuffer::create(bufferSettings));
            mesh.setIndexCount(meshDescription->getIndexCount());
            mesh.setIndexType(meshDescription->getIndexType());
        }

        // Stage transfer.
        meshTransfer->stageCopy(std::move(meshDescription), mesh);

        return mesh;
    }

    MultiMesh& MeshManager::createMultiMeshImpl()
    {
        auto  id      = generateUuid();
        auto  mesh    = std::make_unique<MultiMesh>(*this, id);
        auto& meshRef = *mesh;

        // Make graphics queue the default target family.
        mesh->setTargetFamily(memoryManager->getGraphicsQueue().getFamily());

        assert(meshes.try_emplace(id, std::move(mesh)).second);

        return meshRef;
    }

    SharedMesh& MeshManager::createSharedMesh()
    {
        auto  id      = generateUuid();
        auto  mesh    = std::make_unique<SharedMesh>(*this, id);
        auto& meshRef = *mesh;

        // Make graphics queue the default target family.
        mesh->setTargetFamily(memoryManager->getGraphicsQueue().getFamily());

        assert(meshes.try_emplace(id, std::move(mesh)).second);

        return meshRef;
    }

    void MeshManager::createCustomMeshImpl(IMeshPtr mesh)
    {
        // Make graphics queue the default target family.
        mesh->setTargetFamily(memoryManager->getGraphicsQueue().getFamily());

        assert(meshes.try_emplace(mesh->getUuid(), std::move(mesh)).second);
    }

    bool MeshManager::destroyMesh(const uuids::uuid id)
    {
        const auto mesh = meshes.extract(id);
        if (!mesh) return false;

        staleMeshes.emplace_back(std::move(mesh.mapped()));
        return true;
    }

    uuids::uuid MeshManager::generateUuid() { return uuids::uuid_system_generator{}(); }

    ////////////////////////////////////////////////////////////////
    // Transfer.
    ////////////////////////////////////////////////////////////////

    void MeshManager::deallocateDeletedMeshes()
    {
        // TODO: If meshes still have pending transfers, these should be cancelled/destroyed.
        staleMeshes.clear();
    }

    // TODO: Rename to transfer, same as TextureManager?
    void MeshManager::transferStagedCopies() const { meshTransfer->transfer(); }
}  // namespace sol
