#pragma once

#include <Shard3D/core/ecs/actor.h>
#include <Shard3D/core/ecs/components.h>
#include <Shard3D/core/rendering/window.h>
#include <Shard3D/events/input.h>

namespace Shard3D {
    namespace controller {
	    class EditorMovementController {
	    public:
            void tryPollTranslation(EngineWindow& _window, float _dt, const ECS::Actor& _actor);
            void tryPollOrientation(EngineWindow& _window, float _dt, const ECS::Actor& _actor);

            void eventEvent(Events::Event& e);

            bool keyDownEvent(Events::KeyDownEvent& e);
            bool mouseButtonDownEvent(Events::MouseButtonDownEvent& e);
            bool mouseScrollEvent(Events::MouseScrollEvent& e);
   
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
                int speedUp = GLFW_KEY_LEFT_SHIFT;
            };

            struct ButtonMappings {
                int canRotate = GLFW_MOUSE_BUTTON_RIGHT;
            };
             KeyMappings keys{};
             ButtonMappings buttons{};
             glm::vec3 orientation{ .0f, .0f, 6.28318530718f };
             glm::vec3 upVec{ 0.f, 1.f, 0.f };
             bool firstClick = true;
             ECS::Actor actor;
             float dt;
             GLFWwindow* window;
	    };

    }
}