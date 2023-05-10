#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-memory/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-mesh/fwd.h"

namespace sol
{
    class MeshManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MeshManager();

        explicit MeshManager(MemoryManager& memManager);

        MeshManager(MemoryManager& memManager, IMeshTransferPtr mshTransfer);

        MeshManager(const MeshManager&) = delete;

        MeshManager(MeshManager&&) = delete;

        ~MeshManager();

        MeshManager& operator=(const MeshManager&) = delete;

        MeshManager& operator=(MeshManager&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the MemoryManager.
         * \return MemoryManager.
         */
        MemoryManager& getMemoryManager() noexcept;

        /**
         * \brief Get the MemoryManager.
         * \return MemoryManager.
         */
        [[nodiscard]] const MemoryManager& getMemoryManager() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Layouts.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new mesh layout.
         * \param name Optional name.
         * \return New MeshLayout.
         */
        MeshLayout& createMeshLayout(std::string name);

        /**
         * \brief Retrieve a mesh layout.
         * \param id UUID.
         * \return MeshLayout.
         */
        [[nodiscard]] MeshLayout& getMeshLayout(uuids::uuid id) const;

        /**
         * \brief Destroy the mesh layout with the given name.
         * \param id UUID of the layout to destroy.
         * \return True if layout was destroyed, false if a layout with the given id did not exist.
         */
        bool destroyMeshLayout(uuids::uuid id);

        ////////////////////////////////////////////////////////////////
        // Meshes.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new MeshDescription.
         * \return MeshDescription.
         */
        [[nodiscard]] MeshDescriptionPtr createMeshDescription();

        //FlatMesh& createFlatMesh();

        /**
         * \brief Create a new FlatMesh from a MeshDescription.
         * \param meshDescription MeshDescription. Must contain a valid vertex buffer.
         * \return FlatMesh.
         */
        FlatMesh& createFlatMesh(MeshDescriptionPtr meshDescription);

        void updateFlatMesh(FlatMesh& mesh, MeshDescriptionPtr meshDescription);

        //IndexedMesh& createIndexedMesh();

        /**
         * \brief Create a new IndexedMesh from a MeshDescription.
         * \param meshDescription MeshDescription. Must contain a valid vertex and index buffer.
         * \return IndexedMesh.
         */
        IndexedMesh& createIndexedMesh(MeshDescriptionPtr meshDescription);

        void updateIndexedMesh(IndexedMesh& mesh, MeshDescriptionPtr meshDescription);

        //MultiMesh& createMultiMesh();

        /**
         * \brief Create a new MultiMesh from a MeshDescription.
         * \param meshDescription MeshDescription. Must at least contain a valid vertex buffer.
         * \return MultiMesh.
         */
        MultiMesh& createMultiMesh(MeshDescriptionPtr meshDescription);

        /**
         * \brief Create a new SharedMesh.
         * \return SharedMesh.
         */
        SharedMesh& createSharedMesh();

        /**
         * \brief Create a new mesh of a custom type.
         * \tparam T Mesh type. Should derive from IMesh.
         * \tparam Args Arguments passed to constructor.
         * \param args Arguments passed to constructor.
         * \return CustomMesh.
         */
        template<std::derived_from<IMesh> T, typename... Args>
        T& createCustomMesh(Args&&... args)
        {
            auto  mesh    = std::make_unique<T>(*this, generateUuid(), std::forward<Args>(args)...);
            auto& meshRef = *mesh;

            createCustomMeshImpl(std::move(mesh));

            return meshRef;
        }

        bool destroyMesh(uuids::uuid id);

        ////////////////////////////////////////////////////////////////
        // Transfer.
        ////////////////////////////////////////////////////////////////

        void deallocateDeletedMeshes();

        void transferStagedCopies() const;

    private:
        FlatMesh& createFlatMeshImpl();

        IndexedMesh& createIndexedMeshImpl();

        MultiMesh& createMultiMeshImpl();

        void createCustomMeshImpl(IMeshPtr mesh);

        static uuids::uuid generateUuid();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        MemoryManager* memoryManager = nullptr;

        std::unordered_map<uuids::uuid, MeshLayoutPtr> meshLayouts;

        std::unordered_map<uuids::uuid, IMeshPtr> meshes;

        std::vector<IMeshPtr> staleMeshes;

        IMeshTransferPtr meshTransfer;
    };
}  // namespace sol