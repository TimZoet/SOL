#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/fwd.h"

namespace sol
{
    class RayTracingPipelineCache
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Pipeline
        {
            VulkanRayTracingPipelinePtr pipeline;
        };

    public:
        using PipelineMap = std::unordered_map<const RayTracingMaterial*, Pipeline>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        RayTracingPipelineCache();

        RayTracingPipelineCache(const RayTracingPipelineCache&) = delete;

        RayTracingPipelineCache(RayTracingPipelineCache&&) = delete;

        ~RayTracingPipelineCache() noexcept;

        RayTracingPipelineCache& operator=(const RayTracingPipelineCache&) = delete;

        RayTracingPipelineCache& operator=(RayTracingPipelineCache&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanRayTracingPipeline& getPipeline(const RayTracingMaterial& material) const;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new pipeline for the given material, if one does not exist yet.
         * \param material RayTracingMaterial.
         * \return True if a new pipeline was created, false if one already existed.
         */
        bool createPipeline(const RayTracingMaterial& material);

        ////////////////////////////////////////////////////////////////
        // Destroy.
        ////////////////////////////////////////////////////////////////

        bool destroyPipeline(const RayTracingMaterial& material);

    private:
        static VulkanRayTracingPipelinePtr createPipelineImpl(const RayTracingMaterial& material);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        PipelineMap pipelines;
    };
}  // namespace sol
