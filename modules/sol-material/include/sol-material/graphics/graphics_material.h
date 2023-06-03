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
#include "sol-material/graphics/graphics_material_layout.h"

namespace sol
{
    class ForwardMaterial : public Material
    {
    public:
        friend class ForwardMaterialInstance;

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class CullMode
        {
            None  = 0,
            Front = 1,
            Back  = 2,
            Both  = Front | Back
        };

        enum class FrontFace
        {
            Clockwise        = 1,
            CounterClockwise = 2
        };

        enum class PolygonMode
        {
            Fill  = 0,
            Line  = 1,
            Point = 2
        };

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

        [[nodiscard]] VulkanDevice& getDevice() noexcept override;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept override;

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

        [[nodiscard]] const MaterialLayout& getLayout() const noexcept override;

        [[nodiscard]] const ForwardMaterialLayout& getForwardLayout() const noexcept;

        [[nodiscard]] const MeshLayout* getMeshLayout() const noexcept;

        [[nodiscard]] const std::vector<ForwardMaterialInstance*>& getInstances() const noexcept;

        [[nodiscard]] CullMode getCullMode() const noexcept;

        [[nodiscard]] FrontFace getFrontFace() const noexcept;

        [[nodiscard]] PolygonMode getPolyonMode() const noexcept;

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

        void setCullMode(CullMode value) noexcept;

        void setFrontFace(FrontFace value) noexcept;

        void setPolygonMode(PolygonMode value) noexcept;

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

        // TODO: Do these properties belong in the material or the forward layout?
        // And if they belong here, when should it be possible to change them?
        // Also after layout finalization?
        CullMode cullMode = CullMode::None;

        FrontFace frontFace = FrontFace::CounterClockwise;

        PolygonMode polygonMode = PolygonMode::Fill;

        int32_t layer = 0;
    };
}  // namespace sol