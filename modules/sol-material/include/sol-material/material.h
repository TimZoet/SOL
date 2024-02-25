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

        Material() = delete;

        explicit Material(VulkanDevice& device);

        Material(const Material&) = delete;

        Material(Material&&) = delete;

        virtual ~Material() noexcept;

        Material& operator=(const Material&) = delete;

        Material& operator=(Material&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        [[nodiscard]] virtual MaterialLayout& getLayout() noexcept = 0;

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

        VulkanDevice* device = nullptr;

        /**
         * \brief UUID.
         */
        uuids::uuid uuid;

        /**
         * \brief User friendly name.
         */
        std::string name;
    };

    enum class ECullMode
    {
        None  = 0,
        Front = 1,
        Back  = 2,
        Both  = Front | Back
    };

    enum class EFrontFace
    {
        Clockwise        = 1,
        CounterClockwise = 2
    };

    enum class EPolygonMode
    {
        Fill  = 0,
        Line  = 1,
        Point = 2
    };

    inline auto toVulkanEnum(const ECullMode value)
    {
        switch (value)
        {
        case ECullMode::None: return VK_CULL_MODE_NONE;
        case ECullMode::Front: return VK_CULL_MODE_FRONT_BIT;
        case ECullMode::Back: return VK_CULL_MODE_BACK_BIT;
        case ECullMode::Both: return VK_CULL_MODE_FRONT_AND_BACK;
        }
        return VK_CULL_MODE_NONE;
    }

    inline auto toVulkanEnum(const EFrontFace value)
    {
        switch (value)
        {
        case EFrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
        case EFrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

    inline auto toVulkanEnum(const EPolygonMode value)
    {
        switch (value)
        {
        case EPolygonMode::Fill: return VK_POLYGON_MODE_FILL;
        case EPolygonMode::Line: return VK_POLYGON_MODE_LINE;
        case EPolygonMode::Point: return VK_POLYGON_MODE_POINT;
        }
        return VK_POLYGON_MODE_FILL;
    }
}  // namespace sol
