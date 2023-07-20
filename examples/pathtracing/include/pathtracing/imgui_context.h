#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "pathtracing/global_state.h"

void createImguiContext(GlobalState& state);

void destroyImguiContext();

void imguiFrame(GlobalState& state);

void imguiUpdate(GlobalState& state);
