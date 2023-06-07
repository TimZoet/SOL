#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"

namespace sol
{
    class Material
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Material();

        Material(const Material&) = delete;

        Material(Material&&) = delete;

        virtual ~Material() noexcept;

        Material& operator=(const Material&) = delete;

        Material& operator=(Material&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] virtual VulkanDevice& getDevice() noexcept = 0;

        [[nodiscard]] virtual const VulkanDevice& getDevice() const noexcept = 0;

        [[nodiscard]] virtual const MaterialLayout& getLayout() const noexcept = 0;

        /**
         * \brief Get the UUID.
         * \return UUID.
         */
        [[nodiscard]] const uuids::uuid& getUuid() const noexcept;

        /**
         * \brief Get the user friendly name.
         * \return Name.
         */
        [[nodiscard]] const std::string& getName() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the user friendly name.
         * \param value Name.
         */
        void setName(std::string value);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief UUID.
         */
        uuids::uuid uuid;

        /**
         * \brief User friendly name.
         */
        std::string name;
    };

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

    inline auto toVulkanEnum(const CullMode value)
    {
        switch (value)
        {
        case CullMode::None: return VK_CULL_MODE_NONE;
        case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
        case CullMode::Back: return VK_CULL_MODE_BACK_BIT;
        case CullMode::Both: return VK_CULL_MODE_FRONT_AND_BACK;
        }
        return VK_CULL_MODE_NONE;
    }

    inline auto toVulkanEnum(const FrontFace value)
    {
        switch (value)
        {
        case FrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

    inline auto toVulkanEnum(const PolygonMode value)
    {
        switch (value)
        {
        case PolygonMode::Fill: return VK_POLYGON_MODE_FILL;
        case PolygonMode::Line: return VK_POLYGON_MODE_LINE;
        case PolygonMode::Point: return VK_POLYGON_MODE_POINT;
        }
        return VK_POLYGON_MODE_FILL;
    }
}  // namespace sol
