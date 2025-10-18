#include "vk_window.h"

#include <stdexcept>

namespace VKEngine {
    Window::Window(int width, int height, const std::string& windowName)
    : m_WIDTH(width), m_HEIGHT(height), m_windowName(windowName) {
        initWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(m_windowHandle);
        glfwTerminate();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(m_windowHandle);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, m_windowHandle, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    GLFWwindow* Window::getWindowHandle() const {
        return m_windowHandle;
    }

    void Window::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_windowHandle = glfwCreateWindow(m_WIDTH, m_HEIGHT, m_windowName.c_str(), nullptr, nullptr);
    }
}



