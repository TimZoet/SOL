#include "pathtracing/materials/camera_material.h"

CameraMaterialInstance::CameraMaterialInstance() = default;

CameraMaterialInstance::~CameraMaterialInstance() noexcept = default;

////////////////////////////////////////////////////////////////
// Getters.
////////////////////////////////////////////////////////////////

uint32_t CameraMaterialInstance::getSetIndex() const { return 0; }

bool CameraMaterialInstance::isUniformBufferStale(size_t binding) const { return false; }

const void* CameraMaterialInstance::getUniformBufferData(size_t binding) const { return &data; }

sol::Texture2D* CameraMaterialInstance::getTextureData(size_t binding) const { return nullptr; }

////////////////////////////////////////////////////////////////
// Setters.
////////////////////////////////////////////////////////////////

void CameraMaterialInstance::setProjection(math::mat4x4f projection) { data.projection = std::move(projection); }

void CameraMaterialInstance::setView(math::mat4x4f view) { data.view = std::move(view); }
