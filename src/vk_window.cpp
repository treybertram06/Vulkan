#include "vk_window.h"

#include <stdexcept>

namespace VKEngine {
    Window::Window(int width, int height, const std::string& windowName)
    : m_width(width), m_height(height), m_windowName(windowName) {
        initWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(m_windowHandle);
        glfwTerminate();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(m_windowHandle);
    }

    void Window::createSurface(VkInstance instance) {
        if (glfwCreateWindowSurface(instance, m_windowHandle, nullptr, &m_surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void Window::destroySurface(VkInstance instance) {
        if (m_surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(instance, m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }
    }

    void Window::recreateSurface(VkInstance instance) {
        destroySurface(instance);
        createSurface(instance);
    }

    GLFWwindow* Window::getWindowHandle() const {
        return m_windowHandle;
    }

    void Window::framebufferResizeCallback(GLFWwindow* window_handle, int width, int height) {
        auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window_handle));
        window->m_framebufferResized = true;
        window->m_width = width;
        window->m_height = height;
    }

    void Window::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_windowHandle = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_windowHandle, this);
        glfwSetFramebufferSizeCallback(m_windowHandle, framebufferResizeCallback);
    }
}



