cmake_minimum_required(VERSION 3.25)
project(VulkanSetup LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

add_subdirectory(libs/glfw)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp" "src/*.h" )

add_executable(VulkanSetup ${SOURCES} src/App.cpp src/App.h src/Window/app_window.cpp src/Window/app_window.h src/logger/logger.h)

target_include_directories(VulkanSetup PRIVATE libs/glfw/include)

target_link_libraries(VulkanSetup PRIVATE Vulkan::Vulkan glfw)

