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
    class ImprovedImageTransfer;
    class Texture2D;
    class TextureManager;

    using IImageTransferPtr       = std::unique_ptr<IImageTransfer>;
    using IImageTransferSharedPtr = std::shared_ptr<IImageTransfer>;
    using Image2DPtr              = std::unique_ptr<Image2D>;
    using Image2DSharedPtr        = std::shared_ptr<Image2D>;
    using Texture2DPtr            = std::unique_ptr<Texture2D>;
    using Texture2DSharedPtr      = std::shared_ptr<Texture2D>;
    using TextureManagerPtr       = std::unique_ptr<TextureManager>;
    using TextureManagerSharedPtr = std::shared_ptr<TextureManager>;
}  // namespace sol
