#pragma once

namespace sol
{
    class IForwardMaterialManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IForwardMaterialManager();

        IForwardMaterialManager(const IForwardMaterialManager&) = delete;

        IForwardMaterialManager(IForwardMaterialManager&&) = delete;

        virtual ~IForwardMaterialManager() noexcept;

        IForwardMaterialManager& operator=(const IForwardMaterialManager&) = delete;

        IForwardMaterialManager& operator=(IForwardMaterialManager&&) = delete;
    };
}  // namespace sol
