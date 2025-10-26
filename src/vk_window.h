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
        bool wasWindowResized() { return m_framebufferResized; }
        void resetWindowResizedFlag() { m_framebufferResized = false; }

        void createSurface(VkInstance instance);
        void destroySurface(VkInstance instance);
        void recreateSurface(VkInstance instance);
        VkSurfaceKHR surface() const { return m_surface; }

        GLFWwindow* getWindowHandle() const;
        VkExtent2D getExtent() { return { (uint32_t)m_width, (uint32_t)m_height }; }

    private:
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        void initWindow();

        int m_width;
        int m_height;
        bool m_framebufferResized = false;

        std::string m_windowName;
        GLFWwindow* m_windowHandle;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    };

}