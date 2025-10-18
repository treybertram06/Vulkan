#pragma once

#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>

namespace VKEngine {

    class Window {
    public:
        Window(int width, int height, const std::string& windowName);
        ~Window();

        Window(const Window&) = delete;
        Window &operator=(const Window&) = delete;

        bool shouldClose() const;
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        GLFWwindow* getWindowHandle() const;

    private:
        void initWindow();

        const int m_WIDTH;
        const int m_HEIGHT;
        std::string m_windowName;
        GLFWwindow* m_windowHandle;
    };

}