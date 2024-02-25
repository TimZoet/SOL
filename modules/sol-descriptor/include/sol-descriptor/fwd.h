#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class Descriptor;
    class DescriptorBuffer;
    class DescriptorLayout;

    using DescriptorPtr             = std::unique_ptr<Descriptor>;
    using DescriptorSharedPtr       = std::shared_ptr<Descriptor>;
    using DescriptorBufferPtr       = std::unique_ptr<DescriptorBuffer>;
    using DescriptorBufferSharedPtr = std::shared_ptr<DescriptorBuffer>;
    using DescriptorLayoutPtr       = std::unique_ptr<DescriptorLayout>;
    using DescriptorLayoutSharedPtr = std::shared_ptr<DescriptorLayout>;
}  // namespace sol
