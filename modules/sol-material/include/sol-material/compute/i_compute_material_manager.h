#pragma once

namespace sol
{
    class IComputeMaterialManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IComputeMaterialManager();

        IComputeMaterialManager(const IComputeMaterialManager&) = delete;

        IComputeMaterialManager(IComputeMaterialManager&&) = delete;

        virtual ~IComputeMaterialManager() noexcept;

        IComputeMaterialManager& operator=(const IComputeMaterialManager&) = delete;

        IComputeMaterialManager& operator=(IComputeMaterialManager&&) = delete;
    };
}  // namespace sol
