//
// Created by Oriventi on 8/24/2023.
//

#include "app_window.h"

namespace app {
    Window::Window(int32_t width, int32_t height, const std::string&  title) :
        mWidth{width}, mHeight{height}, mTitle{title}
    {
        initWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //For OpenGL with subsequent call
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(mWidth, mHeight, mTitle.c_str(), nullptr, nullptr);
    }
}
