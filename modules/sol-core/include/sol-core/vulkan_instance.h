#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
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
#include "sol-core/version.h"

namespace sol
{
    /**
     * \brief The VulkanInstance object manages the lifetime of a VkInstance.
     */
    class VulkanInstance
    {
    public:
        /**
         * \brief VulkanInstance settings.
         */
        struct Settings
        {
            /**
             * \brief Application name.
             */
            std::string applicationName;

            /**
             * \brief Application version.
             */
            Version applicationVersion;

            /**
             * \brief List of required instance extensions.
             */
            std::vector<std::string> extensions;

            /**
             * \brief Create a debug messenger. Requires the VK_EXT_debug_utils extension to be enabled.
             */
            bool enableDebugging = false;

            /**
             * \brief Enable validation layers.
             */
            bool enableValidation = false;

            /**
             * \brief List of required validation layers.
             */
            std::vector<std::string> validationLayers;

            [[nodiscard]] bool validate() const noexcept;
        };

        using SettingsPtr = std::unique_ptr<Settings>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        VulkanInstance() = delete;

        VulkanInstance(SettingsPtr settingsPtr, VkInstance vkInstance, VkDebugUtilsMessengerEXT vkDebugMessenger);

        VulkanInstance(const VulkanInstance&) = delete;

        VulkanInstance(VulkanInstance&&) = delete;

        ~VulkanInstance() noexcept;

        VulkanInstance& operator=(const VulkanInstance&) = delete;

        VulkanInstance& operator=(VulkanInstance&&) = delete;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new Vulkan instance.
         * \param settings Settings.
         * \throws VulkanError Thrown if instance creation failed.
         * \return Vulkan instance.
         */
        [[nodiscard]] static VulkanInstancePtr create(Settings settings);

        /**
         * \brief Create a new Vulkan instance.
         * \param settings Settings.
         * \throws VulkanError Thrown if instance creation failed.
         * \return Vulkan instance.
         */
        [[nodiscard]] static VulkanInstanceSharedPtr createShared(Settings settings);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the settings with which this object was created.
         * \return Settings.
         */
        [[nodiscard]] const Settings& getSettings() const noexcept;

        /**
         * \brief Get the instance handle managed by this object.
         * \return Instance handle.
         */
        [[nodiscard]] const VkInstance& get() const noexcept;

    private:
        [[nodiscard]] static std::pair<VkInstance, VkDebugUtilsMessengerEXT> createImpl(const Settings& settings);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Settings with which this object was created.
         */
        SettingsPtr settings;

        /**
         * \brief Vulkan instance.
         */
        VkInstance instance = VK_NULL_HANDLE;

        /**
         * \brief Optional debug messenger.
         */
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    };
}  // namespace sol