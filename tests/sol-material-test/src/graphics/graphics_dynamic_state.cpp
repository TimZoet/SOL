#include "sol-material-test/graphics/graphics_dynamic_state.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-material/graphics/graphics_dynamic_state.h"

void GraphicsDynamicState::operator()()
{
    {
        sol::CullMode state;
        compareEQ(sol::GraphicsDynamicState::StateType::CullMode, state.getType());
        compareEQ(sol::CullMode::Value::None, state.value);
        expectNoThrow([&] { state.value = sol::CullMode::Value::Back; });
        compareEQ(sol::CullMode::Value::Back, state.value);
    }

    {
        sol::FrontFace state;
        compareEQ(sol::GraphicsDynamicState::StateType::FrontFace, state.getType());
        compareEQ(sol::FrontFace::Value::Clockwise, state.value);
        expectNoThrow([&] { state.value = sol::FrontFace::Value::CounterClockwise; });
        compareEQ(sol::FrontFace::Value::CounterClockwise, state.value);
    }

    {
        sol::PolygonMode state;
        compareEQ(sol::GraphicsDynamicState::StateType::PolygonMode, state.getType());
        compareEQ(sol::PolygonMode::Value::Fill, state.value);
        expectNoThrow([&] { state.value = sol::PolygonMode::Value::Point; });
        compareEQ(sol::PolygonMode::Value::Point, state.value);
    }

    {
        sol::Scissor state;
        compareEQ(sol::GraphicsDynamicState::StateType::Scissor, state.getType());
        compareTrue(state.values.empty());
        expectNoThrow([&] { state.values.emplace_back(std::pair{16, 32}, std::pair{1024u, 512u}); });
        compareEQ(1ull, state.values.size());
    }

    {
        sol::Viewport state;
        compareEQ(sol::GraphicsDynamicState::StateType::Viewport, state.getType());
        compareTrue(state.values.empty());
        expectNoThrow([&] { state.values.emplace_back(0.0f, 0.0f, 1024.0f, 512.0f, 0.0f, 1.0f); });
        compareEQ(1ull, state.values.size());
    }
}
