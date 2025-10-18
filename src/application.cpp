#include "application.h"

namespace VKEngine {
    void Application::run() {
        while (!m_window.shouldClose()) {
            glfwPollEvents();
        }
    }
}