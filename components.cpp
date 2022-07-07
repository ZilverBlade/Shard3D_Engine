#include "s3dtpch.h" 
#include "components.hpp"

#include "utils/definitions.hpp"
#include "wb3d/assetmgr.hpp"
namespace Shard3D {
	namespace Components {
		glm::mat4 TransformComponent::mat4() {

			// Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
			// Rotation convention = YXZ tait-bryan angles
			// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix

			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{translation.x, translation.y, translation.z, 1.0f} };
		}

		glm::mat3 TransformComponent::normalMatrix() {
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);

			const glm::vec3 invScale = 1.0f / scale;
			return glm::mat3{
				{
					invScale.x * (c1 * c3 + s1 * s2 * s3),
					invScale.x * (c2 * s3),
					invScale.x * (c1 * s2 * s3 - c3 * s1),
				},
				{
					invScale.y * (c3 * s1 * s2 - c1 * s3),
					invScale.y * (c2 * c3),
					invScale.y * (c1 * c3 * s2 + s1 * s3),
				},
				{
					invScale.z * (c2 * s1),
					invScale.z * (-s2),
					invScale.z * (c1 * c2),
				}, };
		}

		MeshComponent::MeshComponent(const std::string& mdl) {
			cacheFile = mdl; // cacheFile is to be used to store future possible model
			file = mdl;
			type = wb3d::AssetManager::retrieveMesh(mdl)->getType();
		}
		void MeshComponent::reapplyMesh(const std::string& mdl) {
			file = mdl;
			wb3d::AssetManager::emplaceMesh(cacheFile);
			//type = wb3d::AssetManager::retrieveMesh(mdl)->getType();
		}
		BillboardComponent::BillboardComponent(const std::string& tex) {
			cacheFile = tex; // cacheFile is to be used to store future possible model
			file = tex;
		}
		void BillboardComponent::reapplyTexture(const std::string& tex) {
			file = tex;
			wb3d::AssetManager::emplaceTexture(cacheFile);
		}
	}
}