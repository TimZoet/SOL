#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"

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
}  // namespace sol