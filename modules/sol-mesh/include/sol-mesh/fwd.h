#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class DefaultMeshTransfer;
    class FlatMesh;
    class GeometryBufferAllocator;
    class IMesh;
    class IMeshTransfer;
    class IndexBuffer;
    class IndexedMesh;
    class Mesh;
    class MeshCollection;
    class MeshDescription;
    class MeshLayout;
    class MeshManager;
    class MultiMesh;
    class SharedMesh;
    class VertexBuffer;

    using GeometryBufferAllocatorPtr       = std::unique_ptr<GeometryBufferAllocator>;
    using GeometryBufferAllocatorSharedPtr = std::shared_ptr<GeometryBufferAllocator>;
    using IMeshPtr                         = std::unique_ptr<IMesh>;
    using IMeshSharedPtr                   = std::shared_ptr<IMesh>;
    using IMeshTransferPtr                 = std::unique_ptr<IMeshTransfer>;
    using IMeshTransferSharedPtr           = std::shared_ptr<IMeshTransfer>;
    using IndexBufferPtr                   = std::unique_ptr<IndexBuffer>;
    using IndexBufferSharedPtr             = std::shared_ptr<IndexBuffer>;
    using MeshPtr                          = std::unique_ptr<Mesh>;
    using MeshSharedPtr                    = std::shared_ptr<Mesh>;
    using MeshCollectionPtr                = std::unique_ptr<MeshCollection>;
    using MeshCollectionSharedPtr          = std::shared_ptr<MeshCollection>;
    using MeshDescriptionPtr               = std::unique_ptr<MeshDescription>;
    using MeshDescriptionSharedPtr         = std::shared_ptr<MeshDescription>;
    using MeshLayoutPtr                    = std::unique_ptr<MeshLayout>;
    using MeshLayoutSharedPtr              = std::shared_ptr<MeshLayout>;
    using MeshManagerPtr                   = std::unique_ptr<MeshManager>;
    using MeshManagerSharedPtr             = std::shared_ptr<MeshManager>;
    using VertexBufferPtr                  = std::unique_ptr<VertexBuffer>;
    using VertexBufferSharedPtr            = std::shared_ptr<VertexBuffer>;
}  // namespace sol
