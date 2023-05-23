#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"
#include "sol-material/material.h"
#include "sol-material/compute/compute_material_layout.h"

namespace sol
{
    class ComputeMaterial : public Material
    {
    public:
        friend class ComputeMaterialInstance;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputeMaterial() = default;

        explicit ComputeMaterial(VulkanShaderModule& computeModule);

        ComputeMaterial(const ComputeMaterial&) = delete;

        ComputeMaterial(ComputeMaterial&&) = delete;

        ~ComputeMaterial() override;

        ComputeMaterial& operator=(const ComputeMaterial&) = delete;

        ComputeMaterial& operator=(ComputeMaterial&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the IComputeMaterialManager.
         * \return IComputeMaterialManager.
         */
        [[nodiscard]] IComputeMaterialManager& getMaterialManager() noexcept;

        /**
         * \brief Get the IComputeMaterialManager.
         * \return IComputeMaterialManager.
         */
        [[nodiscard]] const IComputeMaterialManager& getMaterialManager() const noexcept;

        [[nodiscard]] VulkanShaderModule& getComputeShader() noexcept;

        [[nodiscard]] const VulkanShaderModule& getComputeShader() const noexcept;

        [[nodiscard]] const ComputeMaterialLayout& getLayout() const noexcept;

        [[nodiscard]] const std::vector<ComputeMaterialInstance*>& getInstances() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IComputeMaterialManager& manager);

        void setComputeShader(VulkanShaderModule& module) noexcept;

    private:
        void addInstance(ComputeMaterialInstance& instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IComputeMaterialManager* materialManager = nullptr;

        VulkanShaderModule* computeShader = nullptr;

        std::vector<ComputeMaterialInstance*> instances;

    protected:
        ComputeMaterialLayout layout;
    };
}  // namespace sol
