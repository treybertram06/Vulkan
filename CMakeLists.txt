cmake_minimum_required(VERSION 4.1)
project(Vulkan)

set(CMAKE_CXX_STANDARD 20)

# Find GLFW using pkg-config (robust across distributions that install it as glfw3)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

# Find Vulkan using CMake's provider. This provides the imported target Vulkan::Vulkan.
find_package(Vulkan REQUIRED)

add_executable(Vulkan src/main.cpp
        src/vk_pipeline.h
        src/vk_pipeline.cpp
        src/vk_window.h
        src/vk_window.cpp
        src/application.h
        src/application.cpp
        src/vk_device.h
        src/vk_device.cpp)

# Include GLFW include directories and link its libraries
target_include_directories(Vulkan PRIVATE ${GLFW_INCLUDE_DIRS})
# Link GLFW and Vulkan. Order doesn't usually matter with modern linkers, but list both.
target_link_libraries(Vulkan PRIVATE ${GLFW_LIBRARIES} Vulkan::Vulkan)

if(NOT TARGET Vulkan)
    message(WARNING "Target Vulkan not found when adding DEBUG definition")
else()
    target_compile_definitions(Vulkan PRIVATE $<$<CONFIG:Debug>:DEBUG>)
endif()
