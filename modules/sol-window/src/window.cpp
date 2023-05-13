#include "sol-window/window.h"

namespace sol
{
    Window::Window(const std::array<int32_t, 2> windowSize, std::string windowName) :
        size(windowSize), name(std::move(windowName))
    {
        initialize();
    }

    Window::~Window() noexcept { destroy(); }

    GLFWwindow* Window::get() const noexcept { return window; }

    void Window::run(const std::function<void()>& pre,
                     const std::function<void()>& loop,
                     const std::function<void()>& post) const
    {
        pre();
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            loop();
        }
        post();
    }

    std::array<int32_t, 2> Window::getFramebufferSize() const noexcept
    {
        int32_t width, height;
        glfwGetFramebufferSize(window, &width, &height);
        return {width, height};
    }

    std::vector<std::string> Window::getRequiredExtensions()
    {
        // Get required extensions.
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions     = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        // Convert to std::string.
        std::vector<std::string> extensions;
        extensions.reserve(glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; i++) extensions.emplace_back(glfwExtensions[i]);

        return extensions;
    }

    void Window::awaitMinimization() const
    {
        // Wait until window is no longer minimized.
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }
    }


    void Window::initialize()
    {
        // Initialize GLFW.
        glfwInit();

        // Set window hints before window creation.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        // Create window.
        window = glfwCreateWindow(size[0], size[1], name.c_str(), nullptr, nullptr);
    }

    void Window::destroy() const
    {
        // Destroy window and terminate.
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}  // namespace sol