#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"
#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-descriptor/descriptor_layout.h"

namespace sol
{
    class Material2
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Material2() = delete;

        Material2(uuids::uuid id, VulkanDevice& device, std::vector<const DescriptorLayout*> layouts);

        Material2(VulkanDevice& device, std::vector<const DescriptorLayout*> layouts);

        Material2(const Material2&) = delete;

        Material2(Material2&&) = delete;

        virtual ~Material2() noexcept = 0;

        Material2& operator=(const Material2&) = delete;

        Material2& operator=(Material2&&) = delete;

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
         * \brief Get the list of descriptor layouts.
         * \return Descriptor layouts.
         */
        [[nodiscard]] const std::vector<const DescriptorLayout*>& getDescriptorLayouts() const noexcept;

        [[nodiscard]] virtual const std::vector<VkPushConstantRange>& getPushConstantRanges() const noexcept = 0;

        [[nodiscard]] size_t getPushConstantSize() const noexcept;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief UUID.
         */
        uuids::uuid uuid;

        VulkanDevice* device = nullptr;

        std::vector<const DescriptorLayout*> descriptorLayouts;

        mutable size_t pushConstantSize = ~0ULL;
    };
}  // namespace sol
