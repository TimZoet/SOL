#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>
#include <functional>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace sol
{
    class Window
    {
    public:
        Window() = delete;

        Window(std::array<int32_t, 2> windowSize, std::string windowName);

        Window(const Window&) = delete;

        Window(Window&&) = delete;

        ~Window() noexcept;

        Window& operator=(const Window&) = delete;

        Window& operator=(Window&&) = delete;

        [[nodiscard]] GLFWwindow* get() const noexcept;

        void run(const std::function<void()>& pre,
                 const std::function<void()>& loop,
                 const std::function<void()>& post) const;

        [[nodiscard]] std::array<int32_t, 2> getFramebufferSize() const noexcept;

        /**
         * \brief Get the list of extensions that is required by GLFW.
         * \return List of extensions.
         */
        static std::vector<std::string> getRequiredExtensions();

        void awaitMinimization() const;

    private:
        void initialize();

        void destroy() const;

        /**
         * \brief Window size.
         */
        std::array<int32_t, 2> size;

        /**
         * \brief Window name.
         */
        std::string name;

        /**
         * \brief Window handle.
         */
        GLFWwindow* window = nullptr;
    };
}  // namespace sol
