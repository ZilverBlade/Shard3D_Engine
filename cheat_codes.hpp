#pragma once
#include <GLFW/glfw3.h>
#include "utils/def_dynamic.hpp"
#include "UI/ImGuiLayer.hpp"
namespace Shard3D {
	class CheatCodes {
    public:
        static void init(GLFWwindow* window) {
            glfwSetKeyCallback(window, key_callback);
        }
    private:
        inline static std::string cheatCodeInput;

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                cheatCodeInput.push_back(key);
            }
            if (cheatCodeInput.find("HOOKERS") != std::string::npos) {
                SHARD3D_TOGGLECONSOLE();
                cheatCodeInput.clear();
            }
        } 
	};
}