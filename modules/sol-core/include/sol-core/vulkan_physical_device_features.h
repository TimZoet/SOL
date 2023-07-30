#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

namespace sol
{
    template<typename F, VkStructureType S>
    struct VulkanPhysicalDeviceFeature
    {
        using feature_t                              = F;
        static constexpr VkStructureType structure_v = S;
    };

    struct RootVulkanPhysicalDeviceFeatures2
    {
        RootVulkanPhysicalDeviceFeatures2() = default;

        RootVulkanPhysicalDeviceFeatures2(const RootVulkanPhysicalDeviceFeatures2&) = delete;

        RootVulkanPhysicalDeviceFeatures2(RootVulkanPhysicalDeviceFeatures2&&) = delete;

        virtual ~RootVulkanPhysicalDeviceFeatures2() noexcept = default;

        RootVulkanPhysicalDeviceFeatures2& operator=(const RootVulkanPhysicalDeviceFeatures2&) = delete;

        RootVulkanPhysicalDeviceFeatures2& operator=(RootVulkanPhysicalDeviceFeatures2&&) = delete;

        template<typename F>
        [[nodiscard]] auto* getAs() noexcept
        {
            return static_cast<typename F::feature_t*>(getImpl(F::structure_v));
        }

        virtual [[nodiscard]] void* getImpl(VkStructureType s) noexcept = 0;

        VkPhysicalDeviceFeatures2 features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
    };

    using VulkanPhysicalDeviceFeatures2Ptr = std::unique_ptr<RootVulkanPhysicalDeviceFeatures2>;


    using VulkanPhysicalDeviceVulkan11Features =
      VulkanPhysicalDeviceFeature<VkPhysicalDeviceVulkan11Features,
                                  VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES>;
    using VulkanPhysicalDeviceVulkan12Features =
      VulkanPhysicalDeviceFeature<VkPhysicalDeviceVulkan12Features,
                                  VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES>;
    using VulkanPhysicalDeviceVulkan13Features =
      VulkanPhysicalDeviceFeature<VkPhysicalDeviceVulkan13Features,
                                  VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES>;
    using VulkanPhysicalDeviceAccelerationStructureFeaturesKHR =
      VulkanPhysicalDeviceFeature<VkPhysicalDeviceAccelerationStructureFeaturesKHR,
                                  VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR>;
    using VulkanPhysicalDeviceRayQueryFeaturesKHR =
      VulkanPhysicalDeviceFeature<VkPhysicalDeviceRayQueryFeaturesKHR,
                                  VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR>;
    using VulkanPhysicalDeviceRayTracingPipelineFeaturesKHR =
      VulkanPhysicalDeviceFeature<VkPhysicalDeviceRayTracingPipelineFeaturesKHR,
                                  VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR>;

    template<typename...>
    struct VulkanPhysicalDeviceFeatures2 : RootVulkanPhysicalDeviceFeatures2
    {
        VulkanPhysicalDeviceFeatures2() = default;

        VulkanPhysicalDeviceFeatures2(const VulkanPhysicalDeviceFeatures2&) = delete;

        VulkanPhysicalDeviceFeatures2(VulkanPhysicalDeviceFeatures2&&) = delete;

        ~VulkanPhysicalDeviceFeatures2() noexcept override = default;

        VulkanPhysicalDeviceFeatures2& operator=(const VulkanPhysicalDeviceFeatures2&) = delete;

        VulkanPhysicalDeviceFeatures2& operator=(VulkanPhysicalDeviceFeatures2&&) = delete;

        [[nodiscard]] void* getImpl(const VkStructureType) noexcept override { return nullptr; }

        template<typename = void>
        [[nodiscard]] VkPhysicalDeviceFeatures2& get() noexcept
        {
            return features;
        }
    };

    template<typename F, typename... Fs>
    struct VulkanPhysicalDeviceFeatures2<F, Fs...> : VulkanPhysicalDeviceFeatures2<Fs...>
    {
        using features_t = F;

        using VulkanPhysicalDeviceFeatures2<Fs...>::get;

        VulkanPhysicalDeviceFeatures2() : VulkanPhysicalDeviceFeatures2<Fs...>(), features(features_t::structure_v)
        {
            VulkanPhysicalDeviceFeatures2<Fs...>::get().pNext = &features;
        }

        VulkanPhysicalDeviceFeatures2(const VulkanPhysicalDeviceFeatures2&) = delete;

        VulkanPhysicalDeviceFeatures2(VulkanPhysicalDeviceFeatures2&&) = delete;

        ~VulkanPhysicalDeviceFeatures2() noexcept override = default;

        VulkanPhysicalDeviceFeatures2& operator=(const VulkanPhysicalDeviceFeatures2&) = delete;

        VulkanPhysicalDeviceFeatures2& operator=(VulkanPhysicalDeviceFeatures2&&) = delete;

        [[nodiscard]] void* getImpl(const VkStructureType s) noexcept override
        {
            if (s == features_t::structure_v) return &features;
            return VulkanPhysicalDeviceFeatures2<Fs...>::getImpl(s);
        }

        template<typename = void>
        [[nodiscard]] typename features_t::feature_t& get() noexcept
        {
            return features;
        }

        typename features_t::feature_t features;
    };
}  // namespace sol
