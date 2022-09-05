#include "../../s3dpch.h"
#include "components.h"

#include "../asset/assetmgr.h"
namespace Shard3D {
	namespace Components {

		struct MatrixCalculator {
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix

			// Matrix corresponds to translate * Rz * Rx * Ry * scale transformation
	// Rotation convention = ZXY tait-bryan angles
			static inline glm::mat4 ZXY(TransformComponent& transform) {
				const float c3 = glm::cos(transform.rotation.z);
				const float s3 = glm::sin(transform.rotation.z);
				const float c2 = glm::cos(transform.rotation.x);
				const float s2 = glm::sin(transform.rotation.x);
				const float c1 = glm::cos(transform.rotation.y);
				const float s1 = glm::sin(transform.rotation.y);

				return glm::mat4(
					{
						transform.scale.x * (-c2 * s1),
						transform.scale.x * (c1 * c2),
						transform.scale.x * (s2),
						0.0f,
					},
					{
						transform.scale.y * (c1 * s3 + c3 * s1 * s2),
						transform.scale.y * (s1 * s3 - c1 * c3 * s2),
						transform.scale.y * (c2 * c3),
						0.0f,
					},
					{
						transform.scale.z * (c1 * c3 - s1 * s2 * s3),
						transform.scale.z * (c3 * s1 + c1 * s2 * s3),
						transform.scale.z * (-c2 * s3),
						0.0f,
					},
					{ transform.translation.x, transform.translation.y, transform.translation.z, 1.0f }
				);
			}
			// Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
	// Rotation convention = YXZ tait-bryan angles
			static inline glm::mat4 YXZ(TransformComponent& transform) {
				const float c3 = glm::cos(transform.rotation.z);
				const float s3 = glm::sin(transform.rotation.z);
				const float c2 = glm::cos(transform.rotation.x);
				const float s2 = glm::sin(transform.rotation.x);
				const float c1 = glm::cos(transform.rotation.y);
				const float s1 = glm::sin(transform.rotation.y);
				return glm::mat4(
					{
						transform.scale.x * (c1 * c3 + s1 * s2 * s3),
						transform.scale.x * (c2 * s3),
						transform.scale.x * (c1 * s2 * s3 - c3 * s1),
						0.0f,
					},
					{
						transform.scale.y * (c3 * s1 * s2 - c1 * s3),
						transform.scale.y * (c2 * c3),
						transform.scale.y * (c1 * c3 * s2 + s1 * s3),
						0.0f,
					},
					{
						transform.scale.z * (c2 * s1),
						transform.scale.z * (-s2),
						transform.scale.z * (c1 * c2),
						0.0f,
					},
					{ transform.translation.x, transform.translation.y, transform.translation.z, 1.0f }
				);
			}
		};

		glm::mat4 TransformComponent::calculateMat4() {
			return MatrixCalculator::YXZ(*this);
		}

		glm::mat3 TransformComponent::calculateNormalMatrix() {
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
		void MeshComponent::validate() {
			auto& model = ResourceHandler::retrieveMesh(asset);
			if (model->buffers.size() == materials.size()) return;
			SHARD3D_WARN("MeshComponent validation failed! Resetting materials...");
			materials = model->materials;
		}
		MeshComponent::MeshComponent(const AssetID& mdl) : asset(mdl.getFile()) {
			auto& model = ResourceHandler::retrieveMesh(mdl);
			materials = model->materials;
		}
		BillboardComponent::BillboardComponent(const AssetID& tex) : asset(tex.getFile()) {}

}
}