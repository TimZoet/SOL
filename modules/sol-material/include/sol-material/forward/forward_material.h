#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-mesh/mesh_layout.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"
#include "sol-material/material.h"
#include "sol-material/forward/forward_material_layout.h"

namespace sol
{
    class ForwardMaterial : public Material
    {
    public:
        friend class ForwardMaterialInstance;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ForwardMaterial() = delete;

        ForwardMaterial(VulkanShaderModule& vertexModule, VulkanShaderModule& fragmentModule);

        ForwardMaterial(const ForwardMaterial&) = delete;

        ForwardMaterial(ForwardMaterial&&) = delete;

        ~ForwardMaterial() override;

        ForwardMaterial& operator=(const ForwardMaterial&) = delete;

        ForwardMaterial& operator=(ForwardMaterial&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the IForwardMaterialManager.
         * \return IForwardMaterialManager.
         */
        [[nodiscard]] IForwardMaterialManager& getMaterialManager() noexcept;

        /**
         * \brief Get the IForwardMaterialManager.
         * \return IForwardMaterialManager.
         */
        [[nodiscard]] const IForwardMaterialManager& getMaterialManager() const noexcept;

        [[nodiscard]] const VulkanShaderModule& getVertexShader() const noexcept;

        [[nodiscard]] const VulkanShaderModule& getFragmentShader() const noexcept;

        [[nodiscard]] const ForwardMaterialLayout& getLayout() const noexcept;

        [[nodiscard]] const MeshLayout* getMeshLayout() const noexcept;

        [[nodiscard]] const std::vector<ForwardMaterialInstance*>& getInstances() const noexcept;

        [[nodiscard]] int32_t getLayer() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IForwardMaterialManager& manager);

        void setVertexShader(VulkanShaderModule& module) noexcept;

        void setFragmentShader(VulkanShaderModule& module) noexcept;

        /**
         * \brief Set mesh layout.
         * \param mLayout MeshLayout. Must be compatible with input attributes and bindings as defined in vertex shader.
         */
        void setMeshLayout(MeshLayout& mLayout);

        void setLayer(int32_t l) noexcept;

    private:
        void addInstance(ForwardMaterialInstance& instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IForwardMaterialManager* materialManager = nullptr;

        VulkanShaderModule* vertexShader = nullptr;

        VulkanShaderModule* fragmentShader = nullptr;

        MeshLayout* meshLayout = nullptr;

        std::vector<ForwardMaterialInstance*> instances;

    protected:
        ForwardMaterialLayout layout;

        int32_t layer = 0;
    };
}  // namespace sol