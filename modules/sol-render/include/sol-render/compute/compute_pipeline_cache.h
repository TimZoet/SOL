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
    class ComputePipelineCache
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Pipeline
        {
            VulkanComputePipelinePtr pipeline;
        };

    public:
        using PipelineMap = std::unordered_map<const ComputeMaterial*, Pipeline>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComputePipelineCache();

        ComputePipelineCache(const ComputePipelineCache&) = delete;

        ComputePipelineCache(ComputePipelineCache&&) = delete;

        ~ComputePipelineCache() noexcept;

        ComputePipelineCache& operator=(const ComputePipelineCache&) = delete;

        ComputePipelineCache& operator=(ComputePipelineCache&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanComputePipeline& getPipeline(const ComputeMaterial& material) const;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new pipeline for the given material, if one does not exist yet.
         * \param material ComputeMaterial.
         * \return True if a new pipeline was created, false if one already existed.
         */
        bool createPipeline(const ComputeMaterial& material);

        ////////////////////////////////////////////////////////////////
        // Destroy.
        ////////////////////////////////////////////////////////////////

        bool destroyPipeline(const ComputeMaterial& material);

    private:
        static VulkanComputePipelinePtr createPipelineImpl(const ComputeMaterial& material);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        PipelineMap pipelines;
    };
}  // namespace sol
