#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-error/sol_error.h"

namespace sol
{
    struct GraphicsDynamicState
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class StateType
        {
            CullMode,
            FrontFace,
            PolygonMode,
            Scissor,
            Viewport
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsDynamicState() = default;

        GraphicsDynamicState(const GraphicsDynamicState&) = default;

        GraphicsDynamicState(GraphicsDynamicState&&) = default;

        virtual ~GraphicsDynamicState() = default;

        GraphicsDynamicState& operator=(const GraphicsDynamicState&) = default;

        GraphicsDynamicState& operator=(GraphicsDynamicState&&) = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual StateType getType() const noexcept = 0;
    };

    struct CullMode final : GraphicsDynamicState
    {
        enum class Value
        {
            None  = 0,
            Front = 1,
            Back  = 2,
            Both  = Front | Back
        };

        [[nodiscard]] StateType getType() const noexcept override { return StateType::CullMode; }

        Value value = Value::None;
    };

    struct FrontFace final : GraphicsDynamicState
    {
        enum class Value
        {
            Clockwise        = 1,
            CounterClockwise = 2
        };

        [[nodiscard]] StateType getType() const noexcept override { return StateType::FrontFace; }

        Value value = Value::Clockwise;
    };

    struct PolygonMode final : GraphicsDynamicState
    {
        enum class Value
        {
            Fill  = 0,
            Line  = 1,
            Point = 2
        };

        [[nodiscard]] StateType getType() const noexcept override { return StateType::PolygonMode; }

        Value value = Value::Fill;
    };

    struct Scissor final : GraphicsDynamicState
    {
        struct Value
        {
            std::pair<int32_t, int32_t>   offset;
            std::pair<uint32_t, uint32_t> extent;
        };

        [[nodiscard]] StateType getType() const noexcept override { return StateType::Scissor; }

        std::vector<Value> values;

    };

    struct Viewport final : GraphicsDynamicState
    {
        struct Value
        {
            float x;
            float y;
            float width;
            float height;
            float minDepth;
            float maxDepth;
        };

        [[nodiscard]] StateType getType() const noexcept override { return StateType::Viewport; }

        std::vector<Value> values;
    };

    inline auto toVulkanEnum(const GraphicsDynamicState::StateType value)
    {
        switch (value)
        {
        case GraphicsDynamicState::StateType::CullMode: return VK_DYNAMIC_STATE_CULL_MODE;
        case GraphicsDynamicState::StateType::FrontFace: return VK_DYNAMIC_STATE_FRONT_FACE;
        case GraphicsDynamicState::StateType::PolygonMode: return VK_DYNAMIC_STATE_POLYGON_MODE_EXT;
        case GraphicsDynamicState::StateType::Scissor: return VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT;
        case GraphicsDynamicState::StateType::Viewport: return VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT;
        }
        return VK_DYNAMIC_STATE_MAX_ENUM;
    }

    inline auto toVulkanEnum(const CullMode::Value value)
    {
        switch (value)
        {
        case CullMode::Value::None: return VK_CULL_MODE_NONE;
        case CullMode::Value::Front: return VK_CULL_MODE_FRONT_BIT;
        case CullMode::Value::Back: return VK_CULL_MODE_BACK_BIT;
        case CullMode::Value::Both: return VK_CULL_MODE_FRONT_AND_BACK;
        }
        return VK_CULL_MODE_NONE;
    }

    inline auto toVulkanEnum(const FrontFace::Value value)
    {
        switch (value)
        {
        case FrontFace::Value::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::Value::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

    inline auto toVulkanEnum(const PolygonMode::Value value)
    {
        switch (value)
        {
        case PolygonMode::Value::Fill: return VK_POLYGON_MODE_FILL;
        case PolygonMode::Value::Line: return VK_POLYGON_MODE_LINE;
        case PolygonMode::Value::Point: return VK_POLYGON_MODE_POINT;
        }
        return VK_POLYGON_MODE_FILL;
    }

    inline auto fromVulkanEnum(const VkDynamicState value)
    {
        switch (value)
        {
        case VK_DYNAMIC_STATE_CULL_MODE: return GraphicsDynamicState::StateType::CullMode;
        case VK_DYNAMIC_STATE_FRONT_FACE: return GraphicsDynamicState::StateType::FrontFace;
        case VK_DYNAMIC_STATE_POLYGON_MODE_EXT: return GraphicsDynamicState::StateType::PolygonMode;
        case VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT: return GraphicsDynamicState::StateType::Scissor;
        case VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT: return GraphicsDynamicState::StateType::Viewport;
        default: throw SolError("Unsupported dynamic state.");
        }
    }

    inline auto fromVulkanEnum(const VkCullModeFlags value)
    {
        switch (value)
        {
        case VK_CULL_MODE_NONE: return CullMode::Value::None;
        case VK_CULL_MODE_FRONT_BIT: return CullMode::Value::Front;
        case VK_CULL_MODE_BACK_BIT: return CullMode::Value::Back;
        case VK_CULL_MODE_FRONT_AND_BACK: return CullMode::Value::Both;
        default: return CullMode::Value::None;
        }
    }

    inline auto fromVulkanEnum(const VkFrontFace value)
    {
        switch (value)
        {
        case VK_FRONT_FACE_CLOCKWISE: return FrontFace::Value::Clockwise;
        case VK_FRONT_FACE_COUNTER_CLOCKWISE: return FrontFace::Value::CounterClockwise;
        default: return FrontFace::Value::Clockwise;
        }
    }

    inline auto fromVulkanEnum(const VkPolygonMode value)
    {
        switch (value)
        {
        case VK_POLYGON_MODE_FILL: return PolygonMode::Value::Fill;
        case VK_POLYGON_MODE_LINE: return PolygonMode::Value::Line;
        case VK_POLYGON_MODE_POINT: return PolygonMode::Value::Point;
        default: return PolygonMode::Value::Fill;
        }
    }
}  // namespace sol
