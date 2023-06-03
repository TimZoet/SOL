#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"

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
}  // namespace sol