#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include <vulkan/vulkan.hpp>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "sol-core/fwd.h"
#include "sol-core/object_ref_setting.h"

namespace shaderc
{
    class Compiler;
}

namespace sol
{
    class VulkanShaderModule
    {
    public:
        enum class ShaderType : uint32_t
        {
            Automatic,
            Compute,
            Fragment,
            Vertex
        };

        /**
         * \brief VulkanShaderModule settings.
         */
        struct Settings
        {
            /**
             * \brief Vulkan device.
             */
            ObjectRefSetting<VulkanDevice> device;

            /**
             * \brief Shader type.
             */
            ShaderType type = ShaderType::Automatic;

            /**
             * \brief Optional shader source filename. Only used for informative purposes.
             */
            std::string filename;

            /**
             * \brief Shader source code. If not empty, this code is compiled.
             */
            std::string source;

            /**
             * \brief Precompiled shader binary code. If source is empty, shader module is created from this code.
             */
            std::vector<std::byte> binary;

            /**
             * \brief Optional compiler. If not set, a new compiler is constructed and destructed for each compilation.
             */
            const shaderc::Compiler* compiler = nullptr;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanShaderModule() = delete;

        VulkanShaderModule(SettingsPtr settingsPtr, VkShaderModule vkShaderModule);

        VulkanShaderModule(const VulkanShaderModule&) = delete;

        VulkanShaderModule(VulkanShaderModule&&) = delete;

        ~VulkanShaderModule() noexcept;

        VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;

        VulkanShaderModule& operator=(VulkanShaderModule&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan shader module.
         * \param settings Settings.
         * \throws VulkanError Thrown if device creation failed.
         * \throws VulkanShaderCompilationError Thrown if compilation failed.
         * \return Vulkan shader module.
         */
        [[nodiscard]] static VulkanShaderModulePtr create(Settings settings);

        /**
         * \brief Create a new Vulkan shader module.
         * \param settings Settings.
         * \throws VulkanError Thrown if device creation failed.
         * \throws VulkanShaderCompilationError Thrown if compilation failed.
         * \return Vulkan shader module.
         */
        [[nodiscard]] static VulkanShaderModuleSharedPtr createShared(Settings settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] VulkanDevice& getDevice() noexcept;

        /**
         * \brief Get the device.
         * \return VulkanDevice.
         */
        [[nodiscard]] const VulkanDevice& getDevice() const noexcept;

        /**
         * \brief Get the shader module handle managed by this object.
         * \return Shader module handle.
         */
        [[nodiscard]] const VkShaderModule& get() const noexcept;

    private:
        [[nodiscard]] static VkShaderModule createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Shader module.
         */
        VkShaderModule shaderModule = VK_NULL_HANDLE;
    };
}  // namespace sol