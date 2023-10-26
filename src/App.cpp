#include "App.h"
#include<vector>
#include<string>
#include <iostream>
#include <algorithm>
#include<map>
#include<set>

void App::run() {
    initVulkan();
    mainLoop();
    cleanUp();
}

void App::initVulkan() {
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
}


void App::createInstance() {
    vk::ApplicationInfo appInfo{};
    appInfo.setPApplicationName(WIN_TITLE);
    appInfo.setApplicationVersion(1);
    appInfo.setPEngineName("No Engine");
    appInfo.setEngineVersion(1);
    appInfo.setApiVersion(VK_API_VERSION_1_3);

    vk::InstanceCreateInfo createInfo{};
    createInfo.pApplicationInfo = &appInfo;

    //list all available extensions
    uint32_t availableExtensionCount = 0;

    if(vk::enumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to load number of available extensions!");
    }
    std::vector<vk::ExtensionProperties> availableExtensions(availableExtensionCount);
    if(vk::enumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data()) != vk::Result::eSuccess){
        throw std::runtime_error("failed to load available extensions!");
    }
    std::cout << ">> Supported extensions: " << std::endl;
    for(const auto& extension : availableExtensions)
    {
        std::cout << '\t' << extension.extensionName << std::endl;
    }

    //platform specific extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.setEnabledExtensionCount(glfwExtensionCount);
    createInfo.setPpEnabledExtensionNames(glfwExtensions);

    checkExtensionsSupport(glfwExtensions, glfwExtensionCount, availableExtensions);

    //validation layer
    createInfo.enabledLayerCount = 0;

    if(vk::createInstance(&createInfo, nullptr, &instance) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to create vulkan instance!");
    }
}

uint32_t App::rateDevice(vk::PhysicalDevice device) {
    uint32_t score = 0;
    vk::PhysicalDeviceProperties properties = device.getProperties();
    vk::PhysicalDeviceFeatures features = device.getFeatures();

    if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
    {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += properties.limits.maxImageDimension2D;

    if(!features.geometryShader)
    {
        return 0;
    }

    QueueFamilyIndices indices = findQueueFamilies(device);

    if(!indices.isComplete())
    {
        std::cout << "No graphics queue family found!" << std::endl;
        return 0;
    }

    return score;
}

void App::createSurface() {

    if(glfwCreateWindowSurface(instance, window.getWindow(), nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

}

void App::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamilty : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.queueFamilyIndex = queueFamilty;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo createInfo{};
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    createInfo.enabledLayerCount = 0;

    vk::Result result = physicalDevice.createDevice(&createInfo, nullptr, &device);
    if(result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create logical device");
    }

    device.getQueue(indices.graphicsFamily.value(), 0, &graphicsQueue);
    device.getQueue(indices.presentFamily.value(), 0, &presentQueue);

}

void App::pickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
    if(physicalDevices.empty())
    {
        throw std::runtime_error("No GPUs found on this machine.");
    }

    //find best device
    std::multimap<uint32_t, vk::PhysicalDevice> gpuCandidates;
    std::cout << ">> Available GPUs:" << std::endl;
    for(const auto& device : physicalDevices)
    {
        std::cout << '\t' << device.getProperties().deviceName << std::endl;
        gpuCandidates.insert(std::make_pair(rateDevice(device), device));
    }

    if(gpuCandidates.rbegin()->first > 0)
    {
        std::cout << ">> Selected " << gpuCandidates.rbegin()->second.getProperties().deviceName << " as active GPU" << std::endl;
        physicalDevice = gpuCandidates.rbegin()->second;
    }else
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void App::mainLoop() {
    while(!window.shouldClose())
    {
        glfwPollEvents();
    }
}

void App::cleanUp() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
    device.destroy();
    instance.destroy();
}

void App::checkExtensionsSupport(const char **glfwExtensions, uint32_t glfwExtensionCount,
                                 const std::vector<vk::ExtensionProperties> &availableExtensions) {
    for(std::size_t i = 0; i < glfwExtensionCount; ++i)
    {
        auto it = std::find_if(availableExtensions.begin(), availableExtensions.end(), [&](const vk::ExtensionProperties& extension){
            for(std::size_t j = 0; j < std::strlen(glfwExtensions[i]); ++j)
            {
                if(glfwExtensions[i][j] != extension.extensionName.at(j))
                    return false;
            }
            return true;
        });
        if(it == availableExtensions.end())
        {
            throw std::runtime_error( "Extension " + std::string(glfwExtensions[i]) + " is not available");
        }
    }
    std::cout << ">> All extensions are available on this system" << std::endl;
}

QueueFamilyIndices App::findQueueFamilies(vk::PhysicalDevice const &device) {
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device.getQueueFamilyProperties();

    for(std::size_t i = 0; i < queueFamilyProperties.size(); ++i)
    {
        if(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        if(device.getSurfaceSupportKHR(i, surface, &presentSupport) != vk::Result::eSuccess){
            throw std::runtime_error("failed to get present extension");
        };

        if(presentSupport) {
            indices.presentFamily = i;
        }

        if(indices.isComplete()) {
            break;
        }
    }

    return indices;
}
