#include <iostream>
#include <ostream>
#include <cstdlib>

#include "application.h"

void force_x11_if_linux();

int main() {
    force_x11_if_linux();

    VKEngine::Application app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

void force_x11_if_linux() {
#ifdef __linux__
    // Force the process to use X11/XWayland instead of Wayland so no special run flags are required.
    unsetenv("WAYLAND_DISPLAY");
    setenv("XDG_SESSION_TYPE", "x11", 1);
    setenv("GDK_BACKEND", "x11", 1);
#endif
}