#pragma once

#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>
#include <optional>

#include "window/app_window.h"


const int32_t WIN_WIDTH = 800;
const int32_t WIN_HEIGHT = 600;
const char* const WIN_TITLE = "Vulkan";

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
};

class App {
public:
    App() = default;

    ~App() = default;

    void run();

private:

    void initVulkan();

    void createInstance();

    static void checkExtensionsSupport(const char** glfwExtensions, uint32_t glfwExtensionCount, const std::vector<vk::ExtensionProperties>& availableExtensions);

    void pickPhysicalDevice();

    void createLogicalDevice();

    static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device);

    void mainLoop();

    void cleanUp();

    static uint32_t rateDevice(vk::PhysicalDevice device);

private:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;

    vk::Queue graphicsQueue;

    app::Window window{WIN_WIDTH, WIN_HEIGHT, WIN_TITLE};
};