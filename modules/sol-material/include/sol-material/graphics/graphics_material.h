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
    class GraphicsMaterial : public Material
    {
    public:
        friend class GraphicsMaterialInstance;

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

        GraphicsMaterial() = delete;

        GraphicsMaterial(VulkanShaderModule& vertexModule, VulkanShaderModule& fragmentModule);

        GraphicsMaterial(const GraphicsMaterial&) = delete;

        GraphicsMaterial(GraphicsMaterial&&) = delete;

        ~GraphicsMaterial() override;

        GraphicsMaterial& operator=(const GraphicsMaterial&) = delete;

        GraphicsMaterial& operator=(GraphicsMaterial&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept override;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept override;

        /**
         * \brief Get the IGraphicsMaterialManager.
         * \return IGraphicsMaterialManager.
         */
        [[nodiscard]] IGraphicsMaterialManager& getMaterialManager() noexcept;

        /**
         * \brief Get the IGraphicsMaterialManager.
         * \return IGraphicsMaterialManager.
         */
        [[nodiscard]] const IGraphicsMaterialManager& getMaterialManager() const noexcept;

        [[nodiscard]] const VulkanShaderModule& getVertexShader() const noexcept;

        [[nodiscard]] const VulkanShaderModule& getFragmentShader() const noexcept;

        [[nodiscard]] const MaterialLayout& getLayout() const noexcept override;

        [[nodiscard]] const GraphicsMaterialLayout& getGraphicsLayout() const noexcept;

        [[nodiscard]] const MeshLayout* getMeshLayout() const noexcept;

        [[nodiscard]] const std::vector<GraphicsMaterialInstance*>& getInstances() const noexcept;

        [[nodiscard]] CullMode getCullMode() const noexcept;

        [[nodiscard]] FrontFace getFrontFace() const noexcept;

        [[nodiscard]] PolygonMode getPolyonMode() const noexcept;

        [[nodiscard]] int32_t getLayer() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IGraphicsMaterialManager& manager);

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
        void addInstance(GraphicsMaterialInstance& instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IGraphicsMaterialManager* materialManager = nullptr;

        VulkanShaderModule* vertexShader = nullptr;

        VulkanShaderModule* fragmentShader = nullptr;

        MeshLayout* meshLayout = nullptr;

        std::vector<GraphicsMaterialInstance*> instances;

    protected:
        GraphicsMaterialLayout layout;

        // TODO: Do these properties belong in the material or the graphics layout?
        // And if they belong here, when should it be possible to change them?
        // Also after layout finalization?
        CullMode cullMode = CullMode::None;

        FrontFace frontFace = FrontFace::CounterClockwise;

        PolygonMode polygonMode = PolygonMode::Fill;

        int32_t layer = 0;
    };
}  // namespace sol