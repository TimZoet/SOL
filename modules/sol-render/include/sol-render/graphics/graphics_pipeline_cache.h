#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-material/fwd.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-render/fwd.h"

namespace sol
{
    class ForwardPipelineCache
    {
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        struct Pipeline
        {
            VulkanGraphicsPipelinePtr pipeline;

            VulkanRenderPass* renderPass = nullptr;
        };

    public:
        using PipelineMap = std::unordered_map<const ForwardMaterial*, std::vector<Pipeline>>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ForwardPipelineCache();

        ForwardPipelineCache(const ForwardPipelineCache&) = delete;

        ForwardPipelineCache(ForwardPipelineCache&&) = delete;

        ~ForwardPipelineCache() noexcept;

        ForwardPipelineCache& operator=(const ForwardPipelineCache&) = delete;

        ForwardPipelineCache& operator=(ForwardPipelineCache&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanGraphicsPipeline& getPipeline(const ForwardMaterial&  material,
                                                          const VulkanRenderPass& renderPass) const;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new pipeline for the given material, if one does not exist yet.
         * \param material ForwardMaterial.
         * \return True if a new pipeline was created, false if one already existed.
         */
        bool createPipeline(const ForwardMaterial& material, VulkanRenderPass& renderPass);

        ////////////////////////////////////////////////////////////////
        // Destroy.
        ////////////////////////////////////////////////////////////////

        bool destroyPipeline(const ForwardMaterial& material);

    private:
        static VulkanGraphicsPipelinePtr createPipelineImpl(const ForwardMaterial& material,
                                                            VulkanRenderPass&      renderPass);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        PipelineMap pipelines;
    };
}  // namespace sol
