#pragma once

namespace sol
{
    class IRayTracingMaterialManager
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        IRayTracingMaterialManager();

        IRayTracingMaterialManager(const IRayTracingMaterialManager&) = delete;

        IRayTracingMaterialManager(IRayTracingMaterialManager&&) = delete;

        virtual ~IRayTracingMaterialManager() noexcept;

        IRayTracingMaterialManager& operator=(const IRayTracingMaterialManager&) = delete;

        IRayTracingMaterialManager& operator=(IRayTracingMaterialManager&&) = delete;
    };
}  // namespace sol
