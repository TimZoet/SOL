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
    class GraphicsPipelineCache
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
        using PipelineMap = std::unordered_map<const GraphicsMaterial*, std::vector<Pipeline>>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GraphicsPipelineCache();

        GraphicsPipelineCache(const GraphicsPipelineCache&) = delete;

        GraphicsPipelineCache(GraphicsPipelineCache&&) = delete;

        ~GraphicsPipelineCache() noexcept;

        GraphicsPipelineCache& operator=(const GraphicsPipelineCache&) = delete;

        GraphicsPipelineCache& operator=(GraphicsPipelineCache&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] VulkanGraphicsPipeline& getPipeline(const GraphicsMaterial& material,
                                                          const VulkanRenderPass& renderPass) const;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new pipeline for the given material, if one does not exist yet.
         * \param material GraphicsMaterial.
         * \return True if a new pipeline was created, false if one already existed.
         */
        bool createPipeline(const GraphicsMaterial& material, VulkanRenderPass& renderPass);

        ////////////////////////////////////////////////////////////////
        // Destroy.
        ////////////////////////////////////////////////////////////////

        bool destroyPipeline(const GraphicsMaterial& material);

    private:
        static VulkanGraphicsPipelinePtr createPipelineImpl(const GraphicsMaterial& material,
                                                            VulkanRenderPass&       renderPass);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        PipelineMap pipelines;
    };
}  // namespace sol
