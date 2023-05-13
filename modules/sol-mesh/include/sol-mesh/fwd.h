#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class DefaultMeshTransfer;
    class FlatMesh;
    class IMesh;
    class IMeshTransfer;
    class IndexedMesh;
    class MeshDescription;
    class MeshLayout;
    class MeshManager;
    class MultiMesh;
    class SharedMesh;

    using IMeshPtr                 = std::unique_ptr<IMesh>;
    using IMeshSharedPtr           = std::shared_ptr<IMesh>;
    using IMeshTransferPtr         = std::unique_ptr<IMeshTransfer>;
    using IMeshTransferSharedPtr   = std::shared_ptr<IMeshTransfer>;
    using MeshDescriptionPtr       = std::unique_ptr<MeshDescription>;
    using MeshDescriptionSharedPtr = std::shared_ptr<MeshDescription>;
    using MeshLayoutPtr            = std::unique_ptr<MeshLayout>;
    using MeshLayoutSharedPtr      = std::shared_ptr<MeshLayout>;
    using MeshManagerPtr           = std::unique_ptr<MeshManager>;
    using MeshManagerSharedPtr     = std::shared_ptr<MeshManager>;
}  // namespace sol
