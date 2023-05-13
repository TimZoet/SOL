#include "sol-material/forward/forward_material_layout.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "sol-core/vulkan_descriptor_set_layout.h"
#include "sol-error/sol_error.h"

namespace sol
{
    ////////////////////////////////////////////////////////////////
    // Subtypes.
    ////////////////////////////////////////////////////////////////

    bool ForwardMaterialLayout::UniformBufferBinding::operator==(const UniformBufferBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && size == rhs.size && count == rhs.count &&
               stages == rhs.stages;
    }

    bool ForwardMaterialLayout::SamplerBinding::operator==(const SamplerBinding& rhs) const noexcept
    {
        return set == rhs.set && binding == rhs.binding && count == rhs.count && stages == rhs.stages;
    }

    bool ForwardMaterialLayout::PushConstant::operator==(const PushConstant& rhs) const noexcept
    {
        return offset == rhs.offset && size == rhs.size && stages == rhs.stages;
    }


    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    ForwardMaterialLayout::ForwardMaterialLayout(VulkanDevice& vkDevice) : device(&vkDevice) {}

    ForwardMaterialLayout::~ForwardMaterialLayout() = default;

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool ForwardMaterialLayout::isFinalized() const noexcept { return finalized; }

    const std::vector<ForwardMaterialLayout::UniformBufferBindingPtr>&
      ForwardMaterialLayout::getUniformBuffers() const noexcept
    {
        return uniformBuffers;
    }

    const std::vector<ForwardMaterialLayout::SamplerBindingPtr>& ForwardMaterialLayout::getSamplers() const noexcept
    {
        return samplers;
    }

    const std::vector<ForwardMaterialLayout::PushConstantPtr>& ForwardMaterialLayout::getPushConstants() const noexcept
    {
        return pushConstants;
    }

    std::pair<const ForwardMaterialLayout::UniformBufferBindingPtr*, size_t>
      ForwardMaterialLayout::getUniformBuffers(const uint32_t set) const
    {
        requireFinalized();

        size_t start = uniformBuffers.size(), end = uniformBuffers.size();
        for (size_t i = 0; i < uniformBuffers.size(); i++)
        {
            if (uniformBuffers[i]->set == set)
            {
                start = i;
                break;
            }
        }

        for (size_t i = start; i < uniformBuffers.size(); i++)
        {
            if (uniformBuffers[i]->set == set + 1)
            {
                end = i;
                break;
            }
        }

        return std::make_pair(uniformBuffers.data() + start, end - start);
    }

    std::pair<const ForwardMaterialLayout::SamplerBindingPtr*, size_t>
      ForwardMaterialLayout::getSamplers(const uint32_t set) const
    {
        requireFinalized();

        size_t start = samplers.size(), end = samplers.size();
        for (size_t i = 0; i < samplers.size(); i++)
        {
            if (samplers[i]->set == set)
            {
                start = i;
                break;
            }
        }

        for (size_t i = start; i < samplers.size(); i++)
        {
            if (samplers[i]->set == set + 1)
            {
                end = i;
                break;
            }
        }

        return std::make_pair(samplers.data() + start, end - start);
    }

    const std::vector<VkPipelineColorBlendAttachmentState>& ForwardMaterialLayout::getColorBlending() const noexcept
    {
        return colorBlending;
    }

    size_t ForwardMaterialLayout::getSetCount() const noexcept
    {
        if (isFinalized()) return finalLayouts.size();

        // Calculate highest set index of samplers and uniform buffers.
        uint32_t max = 0;
        for (const auto& s : samplers) max = std::max(max, s->set);
        for (const auto& b : uniformBuffers) max = std::max(max, b->set);

        return max + static_cast<size_t>(1);
    }

    size_t ForwardMaterialLayout::getUniformBufferCount() const noexcept { return uniformBuffers.size(); }

    size_t ForwardMaterialLayout::getSamplerCount() const noexcept { return samplers.size(); }

    size_t ForwardMaterialLayout::getPushConstantCount() const noexcept { return pushConstants.size(); }

    size_t ForwardMaterialLayout::getPushConstantTotalSize() const noexcept
    {
        if (pushConstants.empty()) return 0;
        return pushConstants.back()->offset + pushConstants.back()->size;
    }

    const std::vector<VulkanDescriptorSetLayoutPtr>& ForwardMaterialLayout::getFinalizedDescriptorSetLayouts() const
    {
        requireFinalized();
        return finalLayouts;
    }

    const std::vector<VkPushConstantRange>& ForwardMaterialLayout::getFinalizedPushConstants() const
    {
        return finalConstants;
    }

    ////////////////////////////////////////////////////////////////
    // Modifiers.
    ////////////////////////////////////////////////////////////////

    ForwardMaterialLayout::UniformBufferBinding& ForwardMaterialLayout::addUniformBuffer()
    {
        requireNonFinalized();
        return *uniformBuffers.emplace_back(std::make_unique<UniformBufferBinding>());
    }

    ForwardMaterialLayout::SamplerBinding& ForwardMaterialLayout::addSampler()
    {
        requireNonFinalized();
        return *samplers.emplace_back(std::make_unique<SamplerBinding>());
    }

    ForwardMaterialLayout::PushConstant& ForwardMaterialLayout::addPushConstant()
    {
        requireNonFinalized();
        return *pushConstants.emplace_back(std::make_unique<PushConstant>());
    }

    void ForwardMaterialLayout::addColorBlending(VkPipelineColorBlendAttachmentState state)
    {
        colorBlending.emplace_back(state);
    }

    void ForwardMaterialLayout::finalize()
    {
        requireNonFinalized();

        // Sort everything by set and binding index.
        std::ranges::sort(uniformBuffers.begin(),
                          uniformBuffers.end(),
                          [](const UniformBufferBindingPtr& lhs, const UniformBufferBindingPtr& rhs) {
                              return lhs->set < rhs->set || lhs->set == rhs->set && lhs->binding < rhs->binding;
                          });
        std::ranges::sort(
          samplers.begin(), samplers.end(), [](const SamplerBindingPtr& lhs, const SamplerBindingPtr& rhs) {
              return lhs->set < rhs->set || lhs->set == rhs->set && lhs->binding < rhs->binding;
          });
        std::ranges::sort(
          pushConstants.begin(), pushConstants.end(), [](const PushConstantPtr& lhs, const PushConstantPtr& rhs) {
              return lhs->offset < rhs->offset;
          });

        // Prepare settings for each set.
        std::vector<VulkanDescriptorSetLayout::Settings> layoutSettings(getSetCount(), {.device = *this->device});

        // Add uniform buffers.
        for (const auto& ub : uniformBuffers)
        {
            layoutSettings[ub->set].bindings.emplace_back(
              ub->binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ub->count, ub->stages, nullptr);
        }

        // Add samplers.
        for (const auto& sampler : samplers)
        {
            layoutSettings[sampler->set].bindings.emplace_back(
              sampler->binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler->count, sampler->stages, nullptr);
        }

        // Create layouts.
        finalLayouts.reserve(layoutSettings.size());
        for (const auto& settings : layoutSettings)
            finalLayouts.emplace_back(VulkanDescriptorSetLayout::create(settings));

        // Create push constants.
        finalConstants.reserve(pushConstants.size());
        for (const auto& pc : pushConstants) finalConstants.emplace_back(pc->stages, pc->offset, pc->size);

        finalized = true;
    }

    void ForwardMaterialLayout::requireFinalized() const
    {
        if (!finalized) throw SolError("ForwardMaterialLayout was not yet finalized.");
    }

    void ForwardMaterialLayout::requireNonFinalized() const
    {
        if (finalized) throw SolError("ForwardMaterialLayout was already finalized.");
    }

    ////////////////////////////////////////////////////////////////
    // Compatibility.
    ////////////////////////////////////////////////////////////////

    bool ForwardMaterialLayout::isPushConstantCompatible(const ForwardMaterialLayout& other) const
    {
        requireFinalized();

        return std::ranges::equal(pushConstants,
                                  other.pushConstants,
                                  [](const PushConstantPtr& lhs, const PushConstantPtr& rhs) { return *lhs == *rhs; });
    }

    uint32_t ForwardMaterialLayout::getDescriptorSetCompatibility(const ForwardMaterialLayout& other) const
    {
        requireFinalized();

        auto set = static_cast<uint32_t>(finalLayouts.size());

        // Get set index of first incompatible uniform buffer.
        if (const auto it = std::ranges::mismatch(
                              uniformBuffers,
                              other.uniformBuffers,
                              {},
                              [](const UniformBufferBindingPtr& ptr) { return *ptr; },
                              [](const UniformBufferBindingPtr& ptr) { return *ptr; })
                              .in1;
            it != uniformBuffers.end())
            set = std::min(set, (*it)->set);

        // Get set index of first incompatible sampler.
        const auto it2 = std::ranges::mismatch(
                           samplers,
                           other.samplers,
                           {},
                           [](const SamplerBindingPtr& ptr) { return *ptr; },
                           [](const SamplerBindingPtr& ptr) { return *ptr; })
                           .in1;

        if (it2 != samplers.end()) { set = std::min(set, (*it2)->set); }

        return set;
    }
}  // namespace sol
