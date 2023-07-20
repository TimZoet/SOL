#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "math/include_all.h"
#include "sol-material/forward/forward_material.h"
#include "sol-material/forward/forward_material_instance.h"
#include "sol-texture/texture2d.h"

struct CameraMaterialData
{
    math::mat4x4f view;
    math::mat4x4f projection;
};

class CameraMaterialInstance final : public sol::ForwardMaterialInstance
{
public:
    CameraMaterialInstance();

    CameraMaterialInstance(const CameraMaterialInstance&) = delete;

    CameraMaterialInstance(CameraMaterialInstance&&) = delete;

    ~CameraMaterialInstance() noexcept override;

    CameraMaterialInstance& operator=(const CameraMaterialInstance&) = delete;

    CameraMaterialInstance& operator=(CameraMaterialInstance&&) = delete;

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

    void setView(math::mat4x4f view);

    void setProjection(math::mat4x4f projection);

private:
    CameraMaterialData data;
};
