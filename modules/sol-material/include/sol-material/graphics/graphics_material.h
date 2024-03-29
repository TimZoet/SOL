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
#include "sol-material/graphics/graphics_material_layout.h"

namespace sol
{
    class GraphicsMaterial : public Material
    {
    public:
        friend class GraphicsMaterialInstance;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsMaterial() = delete;

        explicit GraphicsMaterial(VulkanDevice& device);

        GraphicsMaterial(VulkanShaderModule& vertexModule, VulkanShaderModule& fragmentModule);

        GraphicsMaterial(const GraphicsMaterial&) = delete;

        GraphicsMaterial(GraphicsMaterial&&) = delete;

        ~GraphicsMaterial() override;

        GraphicsMaterial& operator=(const GraphicsMaterial&) = delete;

        GraphicsMaterial& operator=(GraphicsMaterial&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

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

        [[nodiscard]] VulkanShaderModule& getVertexShader() const noexcept;

        [[nodiscard]] VulkanShaderModule& getFragmentShader() const noexcept;

        [[nodiscard]] MaterialLayout& getLayout() noexcept override;

        [[nodiscard]] const MaterialLayout& getLayout() const noexcept override;

        [[nodiscard]] GraphicsMaterialLayout& getGraphicsLayout() noexcept;

        [[nodiscard]] const GraphicsMaterialLayout& getGraphicsLayout() const noexcept;

        [[nodiscard]] const std::vector<GraphicsMaterialInstance*>& getInstances() const noexcept;

        [[nodiscard]] int32_t getLayer() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setMaterialManager(IGraphicsMaterialManager& manager);

        void setVertexShader(VulkanShaderModule& module) noexcept;

        void setFragmentShader(VulkanShaderModule& module) noexcept;

        void setLayer(int32_t l) noexcept;

    private:
        void addInstance(GraphicsMaterialInstance& instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        IGraphicsMaterialManager* materialManager = nullptr;

        VulkanShaderModule* vertexShader = nullptr;

        VulkanShaderModule* fragmentShader = nullptr;

        std::vector<GraphicsMaterialInstance*> instances;

    protected:
        GraphicsMaterialLayout layout;

        int32_t layer = 0;
    };
}  // namespace sol