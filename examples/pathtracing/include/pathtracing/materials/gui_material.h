#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "luna/shader_cache/shader_cache.h"
#include "sol-core/fwd.h"
#include "sol-material/forward/forward_material.h"
#include "sol-material/forward/forward_material_instance.h"
#include "sol-mesh/fwd.h"
#include "sol-texture/texture2d.h"

class GuiMaterial : public sol::ForwardMaterial
{
public:
    GuiMaterial() = default;

    GuiMaterial(sol::VulkanShaderModuleSharedPtr vertexModule, sol::VulkanShaderModuleSharedPtr fragmentModule);

    GuiMaterial(const GuiMaterial&) = delete;

    GuiMaterial(GuiMaterial&&) = delete;

    ~GuiMaterial() noexcept override;

    GuiMaterial& operator=(const GuiMaterial&) = delete;

    GuiMaterial& operator=(GuiMaterial&&) = delete;

    static std::unique_ptr<GuiMaterial> create(sol::VulkanDevice& device, sol::ShaderCache& shaderCache);
};

class GuiMaterialInstance : public sol::ForwardMaterialInstance
{
public:
    GuiMaterialInstance() = default;

    GuiMaterialInstance(sol::Texture2D& tex);

    GuiMaterialInstance(const GuiMaterialInstance&) = delete;

    GuiMaterialInstance(GuiMaterialInstance&&) = delete;

    ~GuiMaterialInstance() noexcept override;

    GuiMaterialInstance& operator=(const GuiMaterialInstance&) = delete;

    GuiMaterialInstance& operator=(GuiMaterialInstance&&) = delete;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    [[nodiscard]] uint32_t getSetIndex() const override;

    [[nodiscard]] bool isUniformBufferStale(size_t binding) const override;

    [[nodiscard]] const void* getUniformBufferData(size_t binding) const override;

    [[nodiscard]] sol::Texture2D* getTextureData(size_t binding) const override;

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void setFontTexture(sol::Texture2D* tex);

private:
    sol::Texture2D* fontTexture = nullptr;
};
