#pragma once

#include "../../core.h"
#include "../rendering/window.h"
namespace Shard3D {
	class CheatCodes {
    public:
        static void init(GLFWwindow* window) {
            //glfwSetKeyCallback(window, key_callback);
        }
    private:
        static inline std::string cheatCodeInput;

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS) {
                cheatCodeInput.push_back(key);
            }
            if (cheatCodeInput.find("HOOKCON") != std::string::npos) {
                cheatCodeInput.clear();
                SHARD3D_NOIMPL;
               //SHARD3D_TOGGLECONSOLE();
            }
            if (cheatCodeInput.find("DUMPER") != std::string::npos) {
                cheatCodeInput.clear();
                SHARD3D_STAT_DUMP_ALL();  
            }
        } 
	};
}