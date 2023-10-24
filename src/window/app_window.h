#pragma once

#include<string_view>
#include<string>
#include<glfw/glfw3.h>

namespace app {
    class Window {
    public:
        Window(int32_t width, int32_t height, const std::string& title);
        ~Window();

        [[nodiscard]] inline bool shouldClose() const {return glfwWindowShouldClose(window); }

    private:

        void initWindow();

        int32_t mWidth;
        int32_t mHeight;
        std::string mTitle;

        GLFWwindow* window;
    };
}
