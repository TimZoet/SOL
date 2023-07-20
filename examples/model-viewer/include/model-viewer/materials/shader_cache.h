#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "luna/shader_cache/shader_cache.h"

sol::ShaderCachePtr loadShaderCache(const std::filesystem::path& file);
