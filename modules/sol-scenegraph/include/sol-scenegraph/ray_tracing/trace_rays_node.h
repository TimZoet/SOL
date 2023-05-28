#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-scenegraph/node.h"

namespace sol
{
    class TraceRaysNode final : public Node
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TraceRaysNode() = default;

        TraceRaysNode(VkStridedDeviceAddressRegionKHR raygen,
                      VkStridedDeviceAddressRegionKHR miss,
                      VkStridedDeviceAddressRegionKHR hit,
                      VkStridedDeviceAddressRegionKHR callable,
                      std::array<uint32_t, 3>         dim);

        TraceRaysNode(const TraceRaysNode&) = delete;

        TraceRaysNode(TraceRaysNode&&) = delete;

        ~TraceRaysNode() noexcept override = default;

        TraceRaysNode& operator=(const TraceRaysNode&) = delete;

        TraceRaysNode& operator=(TraceRaysNode&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type getType() const noexcept override;

        [[nodiscard]] VkStridedDeviceAddressRegionKHR getRaygenSBT() const noexcept;

        [[nodiscard]] VkStridedDeviceAddressRegionKHR getMissSBT() const noexcept;

        [[nodiscard]] VkStridedDeviceAddressRegionKHR getHitSBT() const noexcept;

        [[nodiscard]] VkStridedDeviceAddressRegionKHR getCallableSBT() const noexcept;

        [[nodiscard]] std::array<uint32_t, 3> getDimensions() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setRaygenSBT(VkStridedDeviceAddressRegionKHR sbt) noexcept;

        void setMissSBT(VkStridedDeviceAddressRegionKHR sbt) noexcept;

        void setHitSBT(VkStridedDeviceAddressRegionKHR sbt) noexcept;

        void setCallableSBT(VkStridedDeviceAddressRegionKHR sbt) noexcept;

        void setDimensions(std::array<uint32_t, 3> value) noexcept;

        void setDimensions(uint32_t x, uint32_t y, uint32_t z) noexcept;

        ////////////////////////////////////////////////////////////////
        // Debugging and visualization.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string getVizLabel() const override;

        [[nodiscard]] std::string getVizShape() const override;

        [[nodiscard]] std::string getVizFillColor() const override;

    protected:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        VkStridedDeviceAddressRegionKHR raygenSBT;

        VkStridedDeviceAddressRegionKHR missSBT;

        VkStridedDeviceAddressRegionKHR hitSBT;

        VkStridedDeviceAddressRegionKHR callableSBT;

        std::array<uint32_t, 3> dimensions{0, 0, 0};
    };
}  // namespace sol