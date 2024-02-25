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
#include "sol-texture/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-material/fwd.h"

namespace sol
{
    class MaterialInstance
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MaterialInstance();

        MaterialInstance(const MaterialInstance&) = delete;

        MaterialInstance(MaterialInstance&&) = delete;

        virtual ~MaterialInstance() noexcept;

        MaterialInstance& operator=(const MaterialInstance&) = delete;

        MaterialInstance& operator=(MaterialInstance&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

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

        [[nodiscard]] virtual Material& getMaterial() noexcept = 0;

        [[nodiscard]] virtual const Material& getMaterial() const noexcept = 0;

        [[nodiscard]] virtual uint32_t getSetIndex() const = 0;

        [[nodiscard]] virtual const VulkanTopLevelAccelerationStructure*
            getAccelerationStructureData(size_t binding) const;

        [[nodiscard]] virtual const Texture2D* getCombinedImageSamplerData(size_t binding) const;

        [[nodiscard]] virtual const VulkanBuffer* getStorageBufferData(size_t binding) const;

        [[nodiscard]] virtual const Texture2D* getStorageImageData(size_t binding) const;

        [[nodiscard]] virtual bool isUniformBufferStale(size_t binding) const;

        [[nodiscard]] virtual const void* getUniformBufferData(size_t binding) const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set the user friendly name.
         * \param value Name.
         */
        void setName(std::string value);

    protected:
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
