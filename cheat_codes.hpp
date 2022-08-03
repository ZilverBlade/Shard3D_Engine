#pragma once

#include "utils/def_dynamic.hpp"
#include "UI/ImGuiLayer.hpp"
namespace Shard3D {
	class CheatCodes {
    public:
        static void init(GLFWwindow* window) {
            glfwSetKeyCallback(window, key_callback);
        }
    private:
        _S3D_GVAR std::string cheatCodeInput;

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                cheatCodeInput.push_back(key);
            }
            if (cheatCodeInput.find("HOOKC") != std::string::npos) {
                cheatCodeInput.clear();

                SHARD3D_TOGGLECONSOLE();
            }
            if (cheatCodeInput.find("FUCKTHIS") != std::string::npos) {
                cheatCodeInput.clear();

                SHARD3D_FATAL("LMFAO");
            }
            if (cheatCodeInput.find("ELYTRA19") != std::string::npos) {
                cheatCodeInput.clear();

                SHARD3D_FATAL("I died");
            }
        } 
	};
}