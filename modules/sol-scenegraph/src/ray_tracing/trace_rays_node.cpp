#include "sol-scenegraph/ray_tracing/trace_rays_node.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    TraceRaysNode::TraceRaysNode(const VkStridedDeviceAddressRegionKHR raygen,
                                 const VkStridedDeviceAddressRegionKHR miss,
                                 const VkStridedDeviceAddressRegionKHR hit,
                                 const VkStridedDeviceAddressRegionKHR callable,
                                 const std::array<uint32_t, 3>         dim) :
        raygenSBT(raygen), missSBT(miss), hitSBT(hit), callableSBT(callable), dimensions(dim)
    {
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Node::Type TraceRaysNode::getType() const noexcept { return Type::TraceRays; }

    VkStridedDeviceAddressRegionKHR TraceRaysNode::getRaygenSBT() const noexcept { return raygenSBT; }

    VkStridedDeviceAddressRegionKHR TraceRaysNode::getMissSBT() const noexcept { return missSBT; }

    VkStridedDeviceAddressRegionKHR TraceRaysNode::getHitSBT() const noexcept { return hitSBT; }

    VkStridedDeviceAddressRegionKHR TraceRaysNode::getCallableSBT() const noexcept { return callableSBT; }

    std::array<uint32_t, 3> TraceRaysNode::getDimensions() const noexcept { return dimensions; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void TraceRaysNode::setRaygenSBT(const VkStridedDeviceAddressRegionKHR sbt) noexcept { raygenSBT = sbt; }

    void TraceRaysNode::setMissSBT(const VkStridedDeviceAddressRegionKHR sbt) noexcept { missSBT = sbt; }

    void TraceRaysNode::setHitSBT(const VkStridedDeviceAddressRegionKHR sbt) noexcept { hitSBT = sbt; }

    void TraceRaysNode::setCallableSBT(const VkStridedDeviceAddressRegionKHR sbt) noexcept { callableSBT = sbt; }

    void TraceRaysNode::setDimensions(const std::array<uint32_t, 3> value) noexcept { dimensions = value; }

    void TraceRaysNode::setDimensions(const uint32_t x, const uint32_t y, const uint32_t z) noexcept
    {
        dimensions = {x, y, z};
    }
}  // namespace sol
