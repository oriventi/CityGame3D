#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "App.h"

#define GLFW_INCLUDE_VULKAN

int main() {
    App app{};

    try{
        app.run();
    }catch(const std::exception& e)
    {
        std::cerr << "Error catched with following message: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
