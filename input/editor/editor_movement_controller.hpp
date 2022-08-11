#pragma once

#include "../../wb3d/actor.hpp"
#include "../../components.hpp"
#include "../../engine_window.hpp"
#include "../../input.h"

namespace Shard3D {
    namespace controller {
	class EditorMovementController {
	public:
        void tryPoll(GLFWwindow* _window, float _dt, wb3d::Actor& _actor);

        void eventEvent(Event& e);

        bool keyDownEvent(KeyDownEvent& e);
        bool mouseButtonDownEvent(MouseButtonDownEvent& e);
        bool mouseScrollEvent(MouseScrollEvent& e);
   
        float moveSpeed{ 5.f };
        float speedModifier = 1.f;
        
        float sensitivity{ 15.f };

    private:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_RIGHT_CONTROL;
            int slowDown = GLFW_KEY_LEFT_CONTROL;
        };

        struct ButtonMappings {
            int canRotate = GLFW_MOUSE_BUTTON_RIGHT;
        };
         KeyMappings keys{};
         ButtonMappings buttons{};
         glm::vec3 orientation{ .0f, .0f, 6.28318530718f };
         glm::vec3 upVec{ 0.f, 1.f, 0.f };
         bool firstClick = true;
         wb3d::Actor actor;
         float dt;
         GLFWwindow* window;
	};

    }
}

//	obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
//  obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());
