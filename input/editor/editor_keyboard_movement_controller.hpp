#pragma once

#include "../../wb3d/actor.hpp"
#include "../../components.hpp"
#include "../../engine_window.hpp"

namespace Shard3D {
    namespace controller {
	class EditorKeyboardMovementController {

	public:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_LEFT_SHIFT;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int slowDown = GLFW_KEY_LEFT_CONTROL;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, wb3d::Actor& actor);

        KeyMappings keys{};
        float moveSpeed{ 3.f };
        float lookSpeed{ 1.0f };
	};

    }
}

//	obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
//  obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());