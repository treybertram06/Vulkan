cmake_minimum_required(VERSION 3.28)
project(VulkanApp)

set(CMAKE_CXX_STANDARD 20)

# -----------------------------------------------------------
# Vulkan SDK
# -----------------------------------------------------------
if(APPLE)
    # Add the Vulkan SDK to CMake's search path
    list(APPEND CMAKE_PREFIX_PATH "$ENV{VULKAN_SDK}")
endif()

find_package(Vulkan REQUIRED)

# -----------------------------------------------------------
# GLFW (via pkg-config)
# -----------------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED IMPORTED_TARGET glfw3)

# -----------------------------------------------------------
# Source files
# -----------------------------------------------------------
add_executable(Vulkan
        src/main.cpp
        src/vk_pipeline.cpp src/vk_pipeline.h
        src/vk_window.cpp   src/vk_window.h
        src/application.cpp src/application.h
        src/vk_device.cpp   src/vk_device.h
        src/vk_swapchain.cpp src/vk_swapchain.h
        src/model.h
        src/model.cpp
)

# -----------------------------------------------------------
# Include directories
# -----------------------------------------------------------
target_include_directories(Vulkan PRIVATE
        ${GLFW_INCLUDE_DIRS}
)

# -----------------------------------------------------------
# Link libraries
# -----------------------------------------------------------
target_link_libraries(Vulkan PRIVATE
        PkgConfig::GLFW
        Vulkan::Vulkan
)

# -----------------------------------------------------------
# Debug build definition
# -----------------------------------------------------------
target_compile_definitions(Vulkan PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

# -----------------------------------------------------------
# Helpful output
# -----------------------------------------------------------
message(STATUS "GLFW include dirs: ${GLFW_INCLUDE_DIRS}")
message(STATUS "Using Vulkan SDK: $ENV{VULKAN_SDK}")
