#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

namespace sol
{
    class DefaultImageTransfer;
    class IImageTransfer;
    class Image2D;
    class Image2D2;
    class ImprovedImageTransfer;
    class Sampler2D;
    class Texture2D;
    class Texture2D2;
    class TextureManager;

    using IImageTransferPtr       = std::unique_ptr<IImageTransfer>;
    using IImageTransferSharedPtr = std::shared_ptr<IImageTransfer>;
    using Image2DPtr              = std::unique_ptr<Image2D>;
    using Image2DSharedPtr        = std::shared_ptr<Image2D>;
    using Image2D2Ptr             = std::unique_ptr<Image2D2>;
    using Image2D2SharedPtr       = std::shared_ptr<Image2D2>;
    using Sampler2DPtr            = std::unique_ptr<Sampler2D>;
    using Sampler2DSharedPtr      = std::shared_ptr<Sampler2D>;
    using Texture2DPtr            = std::unique_ptr<Texture2D>;
    using Texture2DSharedPtr      = std::shared_ptr<Texture2D>;
    using Texture2D2Ptr           = std::unique_ptr<Texture2D2>;
    using Texture2D2SharedPtr     = std::shared_ptr<Texture2D2>;
    using TextureManagerPtr       = std::unique_ptr<TextureManager>;
    using TextureManagerSharedPtr = std::shared_ptr<TextureManager>;
}  // namespace sol
