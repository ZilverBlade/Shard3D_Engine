#include "../../s3dpch.h"
#include "camera.h"

namespace Shard3D {

	void EngineCamera::setOrthographicProjection(
		float left, float right, float top, float bottom, float zNear, float zFar) {
		projectionMatrix = glm::mat4{ 1.0f };
		projectionMatrix[0][0] = 2.f / (right - left);
		projectionMatrix[1][1] = 2.f / (bottom - top);
		projectionMatrix[2][2] = 1.f / (zFar - zNear);
		projectionMatrix[3][0] = -(right + left) / (right - left);
		projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
		projectionMatrix[3][2] = -zNear / (zFar - zNear);
	}

	void EngineCamera::setPerspectiveProjection(float fovy, float aspect, float zNear /*Near clipping plane*/, float zFar /*Far clipping plane*/) {
		SHARD3D_ASSERT(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f, "");
		const float tanHalfFovy = tan(fovy / 2.f);
		projectionMatrix = glm::mat4{ 0.0f };
		projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
		projectionMatrix[1][1] = 1.f / (tanHalfFovy);
		projectionMatrix[2][2] = zFar / (zFar - zNear);
		projectionMatrix[2][3] = 1.f;
		projectionMatrix[3][2] = -(zFar * zNear) / (zFar - zNear);
	}
	/* *
* Set camera view based on the camera's position and direction
*/
	void EngineCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
		const glm::vec3 w{ glm::normalize(direction) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, position);
		viewMatrix[3][1] = -glm::dot(v, position);
		viewMatrix[3][2] = -glm::dot(w, position);

		inverseViewMatrix = glm::mat4{ 1.f };
		inverseViewMatrix[0][0] = u.x;
		inverseViewMatrix[0][1] = u.y;
		inverseViewMatrix[0][2] = u.z;
		inverseViewMatrix[1][0] = v.x;
		inverseViewMatrix[1][1] = v.y;
		inverseViewMatrix[1][2] = v.z;
		inverseViewMatrix[2][0] = w.x;
		inverseViewMatrix[2][1] = w.y;
		inverseViewMatrix[2][2] = w.z;
		inverseViewMatrix[3][0] = position.x;
		inverseViewMatrix[3][1] = position.y;
		inverseViewMatrix[3][2] = position.z;
	}
	/* *
* Set camera view based on the camera's position and target
*/
	void EngineCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
		SHARD3D_ASSERT((target - position) != glm::vec3(0), "direction cannot be 0");
		setViewDirection(position, target - position, up);
	}
	/* *
* Set camera view based on the camera's position and rotation
*/
	void EngineCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
		const glm::vec3 _t = { position.x, position.z, position.y };
		const glm::vec3 _r = { rotation.x, rotation.z, rotation.y };
		
		const float c3 = glm::cos(_r.z);
		const float s3 = glm::sin(_r.z);
		const float c2 = glm::cos(_r.x);
		const float s2 = glm::sin(_r.x);
		const float c1 = glm::cos(_r.y);
		const float s1 = glm::sin(_r.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, _t);
		viewMatrix[3][1] = -glm::dot(v, _t);
		viewMatrix[3][2] = -glm::dot(w, _t);

		inverseViewMatrix = glm::mat4{ 1.f };
		inverseViewMatrix[0][0] = u.x;
		inverseViewMatrix[0][1] = u.y;
		inverseViewMatrix[0][2] = u.z;
		inverseViewMatrix[1][0] = v.x;
		inverseViewMatrix[1][1] = v.y;
		inverseViewMatrix[1][2] = v.z;
		inverseViewMatrix[2][0] = w.x;
		inverseViewMatrix[2][1] = w.y;
		inverseViewMatrix[2][2] = w.z;
		inverseViewMatrix[3][0] = _t.x;
		inverseViewMatrix[3][1] = _t.y;
		inverseViewMatrix[3][2] = _t.z;
	}
	void EngineCamera::setViewYXZ(glm::mat4 transformMatrix) {
		inverseViewMatrix = transformMatrix;

		viewMatrix = glm::mat4{ 1.f };

		const glm::vec3 u =  { 
			inverseViewMatrix[0][0],
			inverseViewMatrix[0][1],
			inverseViewMatrix[0][2]
		};
		const glm::vec3 v = { 
			inverseViewMatrix[1][0],
			inverseViewMatrix[1][1],
			inverseViewMatrix[1][2] 
		};
		const glm::vec3 w = { 
			inverseViewMatrix[2][0],
			inverseViewMatrix[2][1],
			inverseViewMatrix[2][2] 
		};

		viewMatrix[0][0] = inverseViewMatrix[0][0];
		viewMatrix[1][0] = inverseViewMatrix[0][1];
		viewMatrix[2][0] = inverseViewMatrix[0][2];
		viewMatrix[0][1] = inverseViewMatrix[1][0];
		viewMatrix[1][1] = inverseViewMatrix[1][1];
		viewMatrix[2][1] = inverseViewMatrix[1][2];
		viewMatrix[0][2] = inverseViewMatrix[2][0];
		viewMatrix[1][2] = inverseViewMatrix[2][1];
		viewMatrix[2][2] = inverseViewMatrix[2][2];
		viewMatrix[3][0] = -glm::dot(u, { inverseViewMatrix[3].x, inverseViewMatrix[3].y, inverseViewMatrix[3].z });
		viewMatrix[3][1] = -glm::dot(v, { inverseViewMatrix[3].x, inverseViewMatrix[3].y, inverseViewMatrix[3].z });
		viewMatrix[3][2] = -glm::dot(w, { inverseViewMatrix[3].x, inverseViewMatrix[3].y, inverseViewMatrix[3].z });
	}
}